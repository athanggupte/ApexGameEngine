#pragma once

#include "Entity.h"

namespace Apex {

	template<typename Component_t, typename... Args>
	void Entity::AddComponent(Args&& ... args)
	{
		APEX_CORE_ASSERT(!HasComponent<Component_t>(), "Entity already has component!");
		m_Scene->m_Registry.emplace<Component_t>(m_EntityId, std::forward<Args>(args)...);
	}
	
	template<typename Component_t>
	bool Entity::HasComponent()
	{
		return m_Scene->m_Registry.has<Component_t>(m_EntityId);
	}
	
	template<typename Component_t>
	Component_t& Entity::GetComponent()
	{
		APEX_CORE_ASSERT(HasComponent<Component_t>(), "Entity does not have component!");
		return m_Scene->m_Registry.get<Component_t>(m_EntityId);
	}
	
	template<typename Component_t>
	void Entity::RemoveComponent()
	{
		APEX_CORE_ASSERT(HasComponent<Component_t>(), "Entity does not have component!");
		m_Scene->m_Registry.remove<Component_t>(m_EntityId);
	}

}
