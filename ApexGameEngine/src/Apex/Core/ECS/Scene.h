#pragma once

#include <entt/entt.hpp>

#include "Apex/Core/Events/Event.h"

namespace Apex {
	
	class Entity; // Forward declaration

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;
		
		Entity CreateEntity(const std::string& name = std::string());
		
		void OnUpdate();
		void OnEvent(Event&);
		
		void Render2D();
		
	private:
		entt::registry m_Registry;
		
		friend class Entity;
	};

}
