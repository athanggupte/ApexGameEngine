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
		
		template<typename Component_t, typename... Args>
		inline void AddComponent(Args&& ... args)
		{
			m_Registry->emplace<Component_t>(m_EntityId, std::forward<Args>(args)...);
		}
		
		template<typename Component_t>
		inline bool HasComponent() const
		{
			return m_Registry->has<Component_t>(m_EntityId);
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
		
	protected:
		Entity(entt::entity id, Scene* scene) : m_EntityId(id), m_Registry(&scene->m_Registry) {}
		
	private:
		entt::entity m_EntityId{ entt::null };
		//Scene* m_Scene = nullptr;
		entt::registry* m_Registry = nullptr;
		
		friend class Scene;
	};
	
}


//#include "Entity.hpp"
