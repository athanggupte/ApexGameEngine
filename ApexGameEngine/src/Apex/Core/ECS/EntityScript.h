#pragma once
#include "Entity.h"
#include "Apex/Application.h"

// TODO: Decide whether to remove this
#include "ScriptFactory.h"

namespace Apex {
	
	class APEX_API EntityScript
	{
	public:
		virtual ~EntityScript() = default;
		EntityScript() = default;
		
		template<typename Component_t>
		decltype(auto) GetComponent()
		{
			return m_Entity.GetComponent<Component_t>();
		}

	protected:
		[[nodiscard]] Entity self() const { return m_Entity; }
		[[nodiscard]] Scene* parentScene() const { return m_Entity.m_Scene; }
		static ResourceManager& GetResourceManager() { return Application::Get().GetResourceManager(); }

		virtual void OnCreate() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnRender(Timestep ts) {}
		virtual void OnPhysicsUpdate(Timestep ts) {}
		virtual void OnDestroy() {}
		// virtual void OnFixedUpdate(FixedTimestep ts) {}

	private:
		Entity m_Entity;
		
		friend class Scene;
	};
	
}
