#include "apex_pch.h"
#include "Entity.h"

#include "Components/BasicComponents.h"

namespace Apex {

	void Entity::Destroy()
	{
		m_Scene->m_Registry.destroy(m_EntityId);
		m_EntityId = entt::null;
	}

	StringHandle Entity::Tag()
	{
	    return GetComponent<TagComponent>().tag;
	}

	TransformComponent& Entity::Transform()
	{
		return GetComponent<TransformComponent>();
	}

}
