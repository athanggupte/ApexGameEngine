#pragma once

#include <entt/entt.hpp>
//#include "Scene.h"

namespace Apex {

	class Scene; // Forward Declaration
	
	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(entt::entity id, Scene* scene) : m_EntityId(id), m_Registry(&scene->m_Registry) {}
		
		template<typename Component_t, typename... Args>
		inline void AddComponent(Args&& ... args)
		{
			m_Registry->emplace<Component_t>(m_EntityId, std::forward<Args>(args)...);
		}
		
		template<typename Component_t>
		inline bool HasComponent()
		{
			return m_Registry->has<Component_t>(m_EntityId);
		}
		
		template<typename Component_t>
		inline decltype(auto) GetComponent()
		{
			return m_Registry->get<Component_t>(m_EntityId);
		}
		
		template<typename... Component_t>
		inline decltype(auto) GetComponents()
		{
			return m_Registry->get<Component_t...>(m_EntityId);
		}
		
		template<typename Component_t>
		inline void RemoveComponent()
		{
			m_Registry->remove<Component_t>(m_EntityId);
		}
		
		inline operator bool() const { return m_EntityId != entt::null; }
		inline operator uint32_t() const { return (uint32_t)m_EntityId; }
		
		inline bool operator == (const Entity& other) const
		{
			return m_EntityId == other.m_EntityId && m_Registry == other.m_Registry;
		}
		
		inline bool operator != (const Entity& other) const
		{
			return !(*this == other);
		}
		
		
	private:
		entt::entity m_EntityId{ entt::null };
		//Scene* m_Scene = nullptr;
		entt::registry* m_Registry = nullptr;
		
		friend class Scene;
	};
	
}


//#include "Entity.hpp"
