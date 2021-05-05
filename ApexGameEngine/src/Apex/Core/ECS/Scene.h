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
		
		void OnUpdate(Timestep ts);
		void OnEvent(Event&);
		void Render2D();
		void OnViewportResize(uint32_t width, uint32_t height);
		
		struct SceneOptions
		{
			entt::entity PrimaryCamera = entt::null;
			uint32_t ViewportWidth = 0u, ViewportHeight = 0u;
		};
		
		void SetPrimaryCamera(const Entity&);
	
	protected:
		template<typename Component_t>
		void OnComponentAdded(Entity entity, Component_t& component);
		
	private:
		entt::registry m_Registry;
		
		friend class Entity;
		friend class SceneHeirarchyPanel;
		
		// Scene rendering
		SceneOptions Options;
		
	};

}
