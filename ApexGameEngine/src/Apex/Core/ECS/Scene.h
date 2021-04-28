#pragma once

#include <entt/entt.hpp>

#include "Apex/Core/Timer.h"
#include "Apex/Core/Events/Event.h"

namespace Apex {
	
	class Scene;
	class Entity; // Forward declaration
	
	class Scene
	{
	public:
		Scene();
		~Scene() = default;
		
		Entity CreateEntity(const std::string& name = std::string());
		
		void OnUpdate(Timestep ts, bool play = false);
		void OnEvent(Event&);
		void Render2D();
		
		struct SceneOptions
		{
			entt::entity PrimaryCamera = entt::null;
		};
		
		void SetPrimaryCamera(const Entity&);
		
	private:
		entt::registry m_Registry;
		
		friend class Entity;
		
		// Scene rendering
		SceneOptions Options;
		
	};

}
