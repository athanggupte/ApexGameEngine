#pragma once

#include <entt/entt.hpp>

#include "Apex/Core/Timer.h"
#include "Apex/Core/Events/Event.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Physics/PhysicsManager.h"

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

		Entity CreateMetaEntity();

		void RemoveEntity(Entity entity);

		Entity GetEntityByGUID(const GUID& guid);
		std::vector<Entity> GetEntitiesByTag(StringHandle tag);

		void OnSetup();
		void OnPlay();
		void OnStop();
		void OnUpdate(Timestep ts);
		void OnEditorUpdate(Timestep ts);
		void OnScriptRender(Timestep ts);
		void OnEvent(Event&);
		void OnViewportResize(uint32_t width, uint32_t height);

		void Render2D();
		void Render3D();

		Ref<Scene> Copy();
		
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

		struct SceneOptions
		{
			entt::entity primaryCamera = entt::null;
			uint32_t viewportWidth = 0u, viewportHeight = 0u;
		};

		struct EnvironmentLighting
		{
			Resource<Texture> skyboxTexture;
			Resource<Shader> skyboxShader;
		};

		PhysicsScene& GetPhysicsScene()
		{
			return m_PhysicsScene;
		}

		EnvironmentLighting& GetEnvironment()
		{
			return m_Environment;
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

		PhysicsScene m_PhysicsScene;
		float m_PhysicsTimeAccumulator = 0.f;
		float m_PhysicsTimestep = 1.f / 30.f;
		
		// Scene rendering
		SceneOptions m_Options;
		EnvironmentLighting m_Environment;
	};

}
