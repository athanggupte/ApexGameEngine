#pragma once
#include "AssetManager.h"

namespace Apex {
	
	// Resource Getters
	
	template<typename Resource_t>
	const Ref<Resource_t>& AssetManager::GetResourceByID(const GUID& guid)
	{
		APEX_CORE_ERROR("<AssetManager> : Resource type " + std::string(typeid(Resource_t).name()) + " not available!");
		return nullptr;
	}
	
	// Library Getters
	
	template<typename Resource_t>
	ResourceLibrary<Resource_t>& AssetManager::GetResourceLibrary()
	{
		APEX_CORE_ERROR("<AssetManager> : Resource type " + std::string(typeid(Resource_t).name()) + " not available!");
		return nullptr;
	}
	
	
	
}
