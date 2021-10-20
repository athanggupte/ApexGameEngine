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
		
		Entity CreateEntity(StringHandle name);
		Entity CreateEntity();

		void OnSetup();
		void OnUpdate(Timestep ts);
		void OnEditorUpdate(Timestep ts);
		void OnEvent(Event&);
		void OnViewportResize(uint32_t width, uint32_t height);
		
		struct SceneOptions
		{
			entt::entity PrimaryCamera = entt::null;
			uint32_t ViewportWidth = 0u, ViewportHeight = 0u;
		};
		
		void SetPrimaryCamera(const Entity&);

		// ECS functionality
		template<typename... Component, typename... Exclude>
		decltype(auto) View(entt::exclude_t<Exclude...> exclude = {})
		{
			return m_Registry.view<Component...>(exclude);
		}

	protected:
		void Render2D();
		void Render3D();

		template<typename Component_t>
		void OnComponentAdded(Entity entity, Component_t& component);
		
	private:
		entt::registry m_Registry;
		
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
		template<typename Archiver, typename... Types>
		friend void Serialize(const Scene* scene, Archiver&& archiver);

		
		// Scene rendering
		SceneOptions Options;
		
	};

}
