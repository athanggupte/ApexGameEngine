#include "apex_pch.h"
#include "ResourceManager.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/Mesh.h"
#include "Apex/Graphics/Material.h"

namespace Apex {

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
				m_TexturePool[index].second = Texture2D::Create(resourceData.sourceFile.string());
				break;
			case ResourceType::SHADER:
				m_ShaderPool[index].second = Shader::Create(resourceData.sourceFile.string());
				break;
			case ResourceType::MESH:
				m_MeshPool[index].second = CreateRef<Mesh>(resourceData.sourceFile.string());
				break;
			case ResourceType::MATERIAL:
				m_MaterialPool[index].second = CreateRef<Material>(resourceData.sourceFile.string());
				break;
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
					m_TexturePool[index].second = Texture2D::Create(resourceData.sourceFile.string());
					break;
				case ResourceType::SHADER:
					m_ShaderPool[index].second = Shader::Create(resourceData.sourceFile.string());
					break;
				case ResourceType::MESH:
					m_MeshPool[index].second = CreateRef<Mesh>(resourceData.sourceFile.string());
					break;
				case ResourceType::MATERIAL:
					m_MaterialPool[index].second = CreateRef<Material>(resourceData.sourceFile.string());
					break;
				case ResourceType::NONE:
				default: APEX_CORE_CRITICAL("Unknown resource type!");
				}
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

	// TODO: DELETE SOON!
	//////// TESTING ONLY //////

	void test_main()
	{
		ResourceManager mgr;

		auto texture = mgr.AddResource<Texture>(RESNAME("test-texture-1"), Texture2D::Create("assets/textures/pusheen-thug-life.png"));
		texture->Bind();
		{
			auto texture1 = mgr.Get<Texture>(RESNAME("test-texture-1"));
			APEX_CORE_TRACE("{}", texture1->GetID());
		}
	}
}
