#pragma once

#include <entt/entt.hpp>

namespace Apex {

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;
	private:
		entt::registry m_Registry;
	};

}
