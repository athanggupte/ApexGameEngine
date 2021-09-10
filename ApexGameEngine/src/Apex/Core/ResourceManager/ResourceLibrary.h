#pragma once

#include <unordered_map>
#include "Apex/Core/Core.h"

namespace Apex {
	
	/*************************************************************************
	                                   PLAN
	 -------------------------------------------------------------------------
	 * Every resource/asset created MUST have a UID
	 * Thus ResourceHandles should be the main exchange format rather than
	   shared pointers.
	 * ResourceHandles are responsible for creation and destruction of their
	   contained resource.
	 * Only one ResourceHandle instantiated for each resource. All others are
	   references/pointers to the same.
	 
	 *************************************************************************/
	

	template<typename Resource_t>
	class ResourceLibrary
	{
	public:
		ResourceLibrary() {}
		ResourceLibrary(const ResourceLibrary<Resource_t>& other)
			: m_Registry(other.m_Registry)
		{}
		
		~ResourceLibrary() = default;
		
// 		template<typename... Args>
// 		void Emplace(const string& name, Args... args);

		void Add(uint64_t id, const Ref<Resource_t>& resource);
		bool Exists(uint64_t id) const;
		
		Ref<Resource_t>& GetResourceByID(uint64_t id);
		
		const Ref<Resource_t>& GetResourceByID(uint64_t id) const;
		
// 		Ref<Resource_t> Load(const std::string& filepath);
// 		Ref<Resource_t> Load(const std::string& name, const std::string& filepath);
		
	private:
		std::unordered_map<uint64_t, Ref<Resource_t>> m_Registry;
	};
	
}

#include "ResourceLibrary.hpp"
