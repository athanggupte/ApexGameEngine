#pragma once

#include "ECSComponent.h"
#include <unordered_map>
#include <type_traits>

namespace Apex::ECS {

	class Registry
	{
		static constexpr uint32_t ECS_MAX_ENTITIES = 1024 * 1024;

	public:
		uint32_t createEntity()
		{
			return numEntity++;
		}

		template <typename ComponentType, typename... Args, typename = std::enable_if<std::is_base_of_v<AbstractComponent, ComponentType>>>
		ComponentType& emplace_back(uint32_t entity, Args... args)
		{
			ComponentType tempComponent{ entity, args... };
			return ComponentType::CREATE_FN(ComponentType::COMPONENT_LIST, entity, tempComponent);
		}

	private:
		static uint32_t numEntity;

		std::vector<std::array<uint32_t, ECS_MAX_ENTITIES>> SPARSE_ENTITIES;
	};

}