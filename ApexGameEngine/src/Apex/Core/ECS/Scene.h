#pragma once

#include <entt/entt.hpp>

#include "Apex/Core/Timer.h"
#include "Apex/Core/Events/Event.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"

namespace Apex {
	
	class Scene;
	class Entity; // Forward declaration
	
	class APEX_API Scene
	{
	public:
		Scene();
		~Scene() = default;
		
		Entity CreateEntity();
		Entity CreateEntity(StringHandle name);
		Entity CreateEntityWithGUID(const GUID& guid);
		Entity CreateEntityWithGUID(const GUID& guid, StringHandle name);

		void RemoveEntity(Entity entity);

		void OnSetup();
		void OnPlay();
		void OnStop();
		void OnUpdate(Timestep ts);
		void OnEditorUpdate(Timestep ts);
		void OnEvent(Event&);
		void OnViewportResize(uint32_t width, uint32_t height);

		void Render2D();
		void Render3D();

		Ref<Scene> Copy();

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

		template<typename Component, typename Func>
		void ComponentAddedCallback() // Attach free function
		{
			m_Registry.on_construct<Component>().template connect<&Func>();
		}

		template<typename Component, auto Func, typename Type>
		void ComponentAddedCallback(Type&& instance) // Attach member function
		{
			m_Registry.on_construct<Component>().template connect<&Func>(instance);
		}

	protected:
		/*template<typename Component_t>
		void OnComponentAdded(Entity entity, Component_t& component);*/

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
