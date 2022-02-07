#include "apex_pch.h"
#include "ResourceManager.h"

#include "Apex/Graphics/FBXImporter.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/Mesh.h"
#include "Apex/Graphics/Material.h"

namespace Apex {

	ResourceManager::ResourceManager(const ResourceManager& other)
		: m_Registry(other.m_Registry), m_DependencyGraph(other.m_DependencyGraph), m_TexturePool(other.m_TexturePool),
		  m_ShaderPool(other.m_ShaderPool), m_ScriptFactoryPool(other.m_ScriptFactoryPool),
		  m_MeshPool(other.m_MeshPool), m_MaterialPool(other.m_MaterialPool)
	{
	}

	bool ResourceManager::Exists(Handle id)
	{
		return m_Registry.find(id) != m_Registry.end();
	}

	void ResourceManager::Load(Handle id)
	{
		const auto itr = m_Registry.find(id);
		APEX_CORE_ASSERT(itr != m_Registry.end(), "Resource '" + TO_STRING(Strings::Get(id)) + "' does NOT exist!");

		auto& [index, resourceData] = itr->second;

		if (!resourceData.sourceFile.empty()) {
			switch (resourceData.type) {
			case ResourceType::FILE: break;
			case ResourceType::TEXTURE:
			{
				Ref<Texture> newTexture = Texture2D::Create(resourceData.sourceFile);
				m_TexturePool[index].second.swap(newTexture);
				break;
			}
			case ResourceType::SHADER:
			{
				Ref<Shader> newShader = Shader::Create(resourceData.sourceFile);
				if (!newShader->IsValid()) return;

				m_ShaderPool[index].second.swap(newShader);
				
				if (newShader && newShader->IsBound())
					m_ShaderPool[index].second->Bind();

				break;
			}
			case ResourceType::SCRIPT:
			{
				break;
			}
			case ResourceType::MESH:
			{
				//FBXImporter::LoadMesh(resourceData.sourceFile, TO_STRING(Strings::Get(id)));
				Ref<Mesh> newMesh = CreateRef<Mesh>(resourceData.sourceFile);
				m_MeshPool[index].second.swap(newMesh);
				break;
			}
			case ResourceType::MATERIAL:
			{
				Ref<Material> newMaterial = CreateRef<Material>(resourceData.sourceFile);
				m_MaterialPool[index].second.swap(newMaterial);
				break;
			}
			case ResourceType::NONE:
			default: APEX_CORE_CRITICAL("Unknown resource type!");
			}
		}
	}

	void ResourceManager::LoadAllResources()
	{
		for (auto& [id, data] : m_Registry) {
			auto& [index, resourceData] = data;

			if (!resourceData.sourceFile.empty()) {
				switch (resourceData.type) {
				case ResourceType::FILE: break;
				case ResourceType::TEXTURE:
				{
					Ref<Texture> newTexture = Texture2D::Create(resourceData.sourceFile.string());
					m_TexturePool[index].second.swap(newTexture);
					break;
				}
				case ResourceType::SHADER:
				{
					Ref<Shader> newShader = Shader::Create(resourceData.sourceFile.string());
					m_ShaderPool[index].second.swap(newShader);
					break;
				}
				case ResourceType::MESH:
				{
					Ref<Mesh> newMesh = CreateRef<Mesh>(resourceData.sourceFile.string());
					m_MeshPool[index].second.swap(newMesh);
					break;
				}
				case ResourceType::MATERIAL:
				{
					Ref<Material> newMaterial = CreateRef<Material>(resourceData.sourceFile.string());
					m_MaterialPool[index].second.swap(newMaterial);
					break;
				}
				case ResourceType::NONE:
				default: APEX_CORE_CRITICAL("Unknown resource type!");
				}
			}
		}
	}

	void ResourceManager::Clear()
	{
		m_Registry.clear();
		m_DependencyGraph.clear();
		m_TexturePool.clear();
		m_ShaderPool.clear();
		m_ScriptFactoryPool.clear();
		m_MeshPool.clear();
		m_MaterialPool.clear();
	}

	void ResourceManager::CreateSnapshot(ResourceManager& snapshotTarget) const
	{
		snapshotTarget.LoadSnapshot(*this);
	}

	void ResourceManager::LoadSnapshot(const ResourceManager& snapshot)
	{
		m_Registry = snapshot.m_Registry;
		m_DependencyGraph = snapshot.m_DependencyGraph;
		m_TexturePool = snapshot.m_TexturePool;
		m_ShaderPool = snapshot.m_ShaderPool;
		m_ScriptFactoryPool = snapshot.m_ScriptFactoryPool;
		m_MeshPool = snapshot.m_MeshPool;
		m_MaterialPool = snapshot.m_MaterialPool;
	}

	template <>
	void ResourceManager::LoadAll<Shader>()
	{
		bool foundBoundShader = false;
		for (auto& [id, shader] : m_ShaderPool) {
			const auto itr = m_Registry.find(id);

			if (!itr->second.resourceData.sourceFile.empty()) {
				Ref<Shader> newShader = Shader::Create(itr->second.resourceData.sourceFile.string());
				if (!newShader->IsValid()) return;

				shader.swap(newShader);
				
				if (newShader && !foundBoundShader && newShader->IsBound()) {
					shader->Bind();
					foundBoundShader = true;
				}
			}
		}
	}

	template <>
	void ResourceManager::LoadAll<Texture>()
	{
		for (auto& [id, texture] : m_TexturePool) {
			const auto itr = m_Registry.find(id);

			if (!itr->second.resourceData.sourceFile.empty()) {
				Ref<Texture> newTexture = Texture2D::Create(itr->second.resourceData.sourceFile.string());
				texture.swap(newTexture);
			}
		}
	}

	void ResourceManager::AddDependency(Handle dependent, Handle dependency)
	{
		const auto it = m_DependencyGraph.find(dependent);
		if (it != m_DependencyGraph.end()) {
			it->second.push_back(dependency);
		}
		else {
			m_DependencyGraph.emplace(dependent, std::list{ dependency });
		}
		m_Unsorted = true;
	}

	Iterable<std::vector<Handle>> ResourceManager::SolveDependencies()
	{
		if (m_Unsorted)
			TopologicalSort();
		return Iterable(m_SortedOrder);
	}


	// Dependency management

	void topologicalSortUtil(Handle node, std::unordered_set<Handle>& visited, std::deque<Handle>& queue, std::unordered_map<Handle, std::list<Handle>>& graph)
	{
		visited.insert(node);

		for (auto& child : graph[node]) {
			if (visited.count(child) == 0)
				topologicalSortUtil(child, visited, queue, graph);
		}

		queue.push_back(node);
	}

	void ResourceManager::TopologicalSort()
	{
		std::unordered_set<Handle> visited(m_DependencyGraph.size());
		std::deque<Handle> queue;
		for (auto& [node, adj] : m_DependencyGraph)
			if (visited.count(node) == 0)
				topologicalSortUtil(node, visited, queue, m_DependencyGraph);

		m_SortedOrder.assign(queue.begin(), queue.end());
		m_Unsorted = false;
	}

}
