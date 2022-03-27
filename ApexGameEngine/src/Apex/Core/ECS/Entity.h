#pragma once

#include <entt/entt.hpp>
#include "Scene.h"

namespace Apex {

	struct TransformComponent;
	// class Scene; // Forward Declaration
	
	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(entt::entity id, Scene* scene) : m_EntityId(id), m_Scene(scene) {}

		void Destroy();

		template<typename Component_t, typename... Args>
		inline Component_t& AddComponent(Args&& ... args)
		{
			auto& component = m_Scene->m_Registry.emplace<Component_t>(m_EntityId, std::forward<Args>(args)...);
			//m_Scene->OnComponentAdded<Component_t>(*this, component);
			return component;
		}
		
		template<typename Component_t>
		inline bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<Component_t>(m_EntityId);
		}
		
		template<typename Component_t>
		inline decltype(auto) GetComponent()
		{
			return m_Scene->m_Registry.get<Component_t>(m_EntityId);
		}
		
		template<typename... Component_t>
		inline decltype(auto) GetComponents()
		{
			return m_Scene->m_Registry.get<Component_t...>(m_EntityId);
		}
		
		template<typename Component_t>
		inline void RemoveComponent()
		{
			m_Scene->m_Registry.remove<Component_t>(m_EntityId);
		}

		template<typename... Component_t>
		inline void RemoveComponents()
		{
			m_Scene->m_Registry.remove<Component_t...>(m_EntityId);
		}

		inline operator bool() const { return m_EntityId != entt::null; }
		inline operator entt::entity() const { return m_EntityId; }
		inline operator uint32_t() const { return (uint32_t)m_EntityId; }

		inline Scene* GetScene() const { return m_Scene; }

		inline bool operator == (const Entity& other) const
		{
			return m_EntityId == other.m_EntityId && m_Scene == other.m_Scene;
		}
		
		inline bool operator != (const Entity& other) const
		{
			return !(*this == other);
		}

		// User API functions
		APEX_API StringHandle Tag();
		APEX_API TransformComponent& Transform();
		
	private:
		entt::entity m_EntityId{ entt::null };
		Scene* m_Scene = nullptr;
		// entt::registry* m_Registry = nullptr;
		
		friend class Scene;
		friend class EntityScript;
	};
	
}


//#include "Entity.hpp"
