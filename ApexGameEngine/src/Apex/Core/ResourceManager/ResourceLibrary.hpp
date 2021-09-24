#pragma once
#include "ResourceManager.h"

#include <string>
#include <typeinfo>
#include "ResourceLibrary.h"

namespace Apex {

	template<typename Resource_t>
	void ResourceLibrary<Resource_t>::Add(uint64_t id, const Ref<Resource_t>& resource)
	{
		APEX_CORE_ASSERT(!Exists(id), "Resource already exists!");
		m_Registry[id] = resource;
	}

	template<typename Resource_t>
	inline bool ResourceLibrary<Resource_t>::Exists(uint64_t id) const
	{
		return m_Registry.find(id) != m_Registry.end();
	}

	template<typename Resource_t>
	inline Ref<Resource_t>& ResourceLibrary<Resource_t>::GetResourceByID(uint64_t id)
	{
		auto itr = m_Registry.find(id);
		APEX_CORE_ASSERT(itr != m_Registry.end(), "Resource does not exist!");
		return itr->second;
	}

	template<typename Resource_t>
	inline const Ref<Resource_t>& ResourceLibrary<Resource_t>::GetResourceByID(uint64_t id) const
	{
		auto itr = m_Registry.find(id);
		APEX_CORE_ASSERT(itr != m_Registry.end(), "Resource does not exist!");
		return itr->second;
	}
	
}
