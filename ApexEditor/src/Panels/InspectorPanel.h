#pragma once

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"

namespace Apex {
	
	class InspectorPanel
	{
	public:
		InspectorPanel() = default;
		InspectorPanel(Entity entity, const Ref<Scene>& scene);
		
		void SetContext(Entity entity, const Ref<Scene>& scene);
		std::tuple<Entity, const Ref<Scene>&> GetContext() { return std::make_tuple(m_ContextEntity, m_ContextScene); }
		
		void OnImGuiRender();
		
	protected:
		void DrawComponents();
		
	private:
		Ref<Scene> m_ContextScene;
		Entity m_ContextEntity;
	};
	
}
