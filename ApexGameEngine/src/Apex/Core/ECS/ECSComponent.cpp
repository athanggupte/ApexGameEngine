#include "apex_pch.h"
#include "ECSComponent.h"

#include <new>

namespace Apex::ECS {


	uint32_t AbstractComponent::RegisterComponentType(const std::string& typeName)
	{
		auto index = componentTypes.size();
		componentTypes.emplace_back(typeName);
		return index;
	}

}