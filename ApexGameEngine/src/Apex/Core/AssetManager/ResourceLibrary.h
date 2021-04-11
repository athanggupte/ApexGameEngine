#pragma once

#include <unordered_map>
#include "Apex/Core/Core.h"

namespace Apex {
	
	/*************************************************************************
	                                   PLAN
	 -------------------------------------------------------------------------
	 * Every resource/asset created MUST have a GUID
	 * Thus ResourceHandles should be the main exchange format rather than
	   shared pointers.
	 * ResourceHandles are responsible for creation and destruction of their
	   contained resource.
	 * Only one ResourceHandle instantiated for each resource. All others are
	   references/pointers to the same.
	 
	 -> Make ResourceHandle using Ref<T> = std::shared_ptr<ResourceHandle<T>>;
	 -> ResourceHandle<Resource_t> { GUID, Resource_t * }
	 
	 *************************************************************************/
	

	template<typename Resource_t>
	class ResourceLibrary
	{
	public:
		ResourceLibrary() {}
		ResourceLibrary(const ResourceLibrary<Resource_t>& other)
			: m_NameRegistry(other.m_NameRegistry), m_ResourceRegistry(other.m_ResourceRegistry)
		{}
		
		~ResourceLibrary() = default;
		
// 		template<typename... Args>
// 		void Emplace(const string& name, Args... args);
		
		void Add(const std::string& name, const Ref<Resource_t>& resource);
		void Add(const Ref<Resource_t>& resource);
		
		bool Exists(const std::string& name) const;
		bool Exists(const GUID& guid) const;
		
		Ref<Resource_t>& GetResourceByID(const GUID&);
		Ref<Resource_t>& GetResourceByName(const std::string&);
		
		const Ref<Resource_t>& GetResourceByID(const GUID&) const;
		const Ref<Resource_t>& GetResourceByName(const std::string&) const;
		
// 		Ref<Resource_t> Load(const std::string& filepath);
// 		Ref<Resource_t> Load(const std::string& name, const std::string& filepath);
		
	private:
		std::unordered_map<std::string, GUID> m_NameRegistry;
		std::unordered_map<GUID, Ref<Resource_t>> m_ResourceRegistry;
	};
	
}

#include "ResourceLibrary.hpp"
