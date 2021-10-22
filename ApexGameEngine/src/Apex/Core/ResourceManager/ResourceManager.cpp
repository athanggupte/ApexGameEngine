#include "apex_pch.h"
#include "ResourceManager.h"

namespace Apex {
	
	// Load
	void Resource::_LoadResource::operator() (const Ref<VFS::IFile>& texture)
	{
		APEX_LOG_INFO("Loading file '{}'", resource.m_SourceFile);
	}
	
	void Resource::_LoadResource::operator() (const Ref<Texture>& texture)
	{
		APEX_LOG_INFO("Loading texture from '{}'", resource.m_SourceFile);
		resource.m_Ptr = Texture2D::Create(resource.m_SourceFile.string());
	}
	
	void Resource::_LoadResource::operator() (const Ref<Shader>& texture)
	{
		APEX_LOG_INFO("Loading shader from '{}'", resource.m_SourceFile);
		resource.m_Ptr = Shader::Create(resource.m_SourceFile.string());
	}

	// Reload
	void Resource::_ReloadResource::operator() (const Ref<VFS::IFile>& texture)
	{
		APEX_LOG_INFO("Reloading file '{}'", resource.m_SourceFile);
	}

	void Resource::_ReloadResource::operator() (const Ref<Texture>& texture)
	{
		APEX_LOG_INFO("Reloading texture from '{}'", resource.m_SourceFile);
		resource.m_Ptr = Texture2D::Create(resource.m_SourceFile.string());
	}

	void Resource::_ReloadResource::operator() (const Ref<Shader>& texture)
	{
		APEX_LOG_INFO("Reloading shader from '{}'", resource.m_SourceFile);
		resource.m_Ptr = Shader::Create(resource.m_SourceFile.string());
	}

	// Unload
	void Resource::_UnloadResource::operator() (const Ref<VFS::IFile>& texture)
	{
		APEX_LOG_INFO("Unloading file '{}'", resource.m_SourceFile);
	}

	void Resource::_UnloadResource::operator() (const Ref<Texture>& texture)
	{
		APEX_LOG_INFO("Unloading texture from '{}'", resource.m_SourceFile);
		resource.m_Ptr = Ref<Texture>(nullptr);
	}

	void Resource::_UnloadResource::operator() (const Ref<Shader>& texture)
	{
		APEX_LOG_INFO("Unloading shader from '{}'", resource.m_SourceFile);
		resource.m_Ptr = Ref<Shader>(nullptr);
	}


	// Resource management
	Resource* ResourceManager::Get(Handle id)
	{
		auto itr = m_Registry.find(id);
		// APEX_CORE_ASSERT(itr != m_Registry.end(), "Resource '" + std::to_string(id) + "' already exists!");
		if (itr != m_Registry.end())
			return &itr->second;
		return nullptr;
	}

	const Resource* ResourceManager::Get(Handle id) const
	{
		auto itr = m_Registry.find(id);
		// APEX_CORE_ASSERT(itr != m_Registry.end(), "Resource '" + std::to_string(id) + "' already exists!");
		if (itr != m_Registry.end())
			return &itr->second;
		return nullptr;
	}

	bool ResourceManager::Exists(Handle id)
	{
		return m_Registry.find(id) != m_Registry.end();
	}

	void ResourceManager::AddDependency(Handle dependent, Handle dependency)
	{
		auto it = m_DependencyGraph.find(dependent);
		if (it != m_DependencyGraph.end()) {
			it->second.push_back(dependency);
		}
		else {
			m_DependencyGraph.emplace(dependent, std::list<Handle>{ dependency });
		}
		m_Unsorted = true;
	}

	Iterable<std::vector<Handle>> ResourceManager::SolveDependencies()
	{
		if (m_Unsorted)
			TopologicalSort();
		return Iterable<std::vector<Handle>>(m_SortedOrder);
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
