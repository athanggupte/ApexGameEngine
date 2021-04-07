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
		void AddComponent(Args&& ... args);
		
		template<typename Component_t>
		bool HasComponent();
		
		template<typename Component_t>
		Component_t& GetComponent(); 
		
		template<typename Component_t>
		void RemoveComponent(); 
		
		operator bool() const { return m_EntityId != entt::null; }
		
	protected:
		Entity(entt::entity id, Scene* scene) : m_EntityId(id), m_Scene(scene) {}
		
	private:
		entt::entity m_EntityId{ entt::null };
		Scene* m_Scene = nullptr;
		
		friend class Scene;
	};
	
}


#include "Entity.hpp"
