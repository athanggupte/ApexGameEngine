#include "apex_pch.h"
#include "Entity.h"

#include "Components/BasicComponents.h"

namespace Apex {

	StringHandle Entity::Tag()
	{
	    return GetComponent<TagComponent>().tag;
	}

	TransformComponent& Entity::Transform()
	{
		return GetComponent<TransformComponent>();
	}

}
