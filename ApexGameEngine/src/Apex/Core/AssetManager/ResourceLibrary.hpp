#pragma once
#include "AssetManager.h"

#include <string>
#include <typeinfo>

namespace Apex {
	
	template<typename Resource_t>
	void ResourceLibrary<Resource_t>::Add(const std::string& name, const Ref<Resource_t>& resource)
	{
		APEX_CORE_ASSERT(!Exists(name), "Resource already exists");
		m_NameRegistry[name] = resource->GetGUID();
		m_ResourceRegistry[resource->GetGUID()] = resource;
	}
	
	template<typename Resource_t>
	void ResourceLibrary<Resource_t>::Add(const Ref<Resource_t>& resource)
	{
		Add("Unnamed" + std::string(typeid(Resource_t).name()) + "-" + std::to_string(m_NameRegistry.size()), resource);
	}
	
	template<typename Resource_t>
	bool ResourceLibrary<Resource_t>::Exists(const std::string& name) const
	{
		return m_NameRegistry.find(name) != m_NameRegistry.end();
	}
	
	template<typename Resource_t>
	bool ResourceLibrary<Resource_t>::Exists(const GUID& guid) const
	{
		return m_ResourceRegistry.find(guid) != m_ResourceRegistry.end();
	}
	
	template<typename Resource_t>
	Ref<Resource_t>& ResourceLibrary<Resource_t>::GetResourceByID(const GUID& guid)
	{
		auto itr = m_ResourceRegistry.find(guid);
		APEX_CORE_ASSERT(itr != m_ResourceRegistry.end(), "Resource does not exist!");
		return itr->second;
	}
	
	template<typename Resource_t>
	Ref<Resource_t>& ResourceLibrary<Resource_t>::GetResourceByName(const std::string& name)
	{
		auto itr = m_NameRegistry.find(name);
		APEX_CORE_ASSERT(itr != m_NameRegistry.end(), "Resource does not exist!");
		auto itr2 = m_ResourceRegistry.find(itr->second);
		APEX_CORE_ASSERT(itr2 != m_ResourceRegistry.end(), "Resource does not exist!");
		return itr2->second;
	}
	
	template<typename Resource_t>
	const Ref<Resource_t>& ResourceLibrary<Resource_t>::GetResourceByID(const GUID& guid) const
	{
		const auto itr = m_ResourceRegistry.find(guid);
		APEX_CORE_ASSERT(itr != m_ResourceRegistry.end(), "Resource does not exist!");
		return itr->second;
	}
	
	template<typename Resource_t>
	const Ref<Resource_t>& ResourceLibrary<Resource_t>::GetResourceByName(const std::string& name) const
	{
		const auto itr = m_NameRegistry.find(name);
		APEX_CORE_ASSERT(itr != m_NameRegistry.end(), "Resource does not exist!");
		auto itr2 = m_ResourceRegistry.find(itr->second);
		APEX_CORE_ASSERT(itr2 != m_ResourceRegistry.end(), "Resource does not exist!");
		return itr2->second;
	}
	
}
