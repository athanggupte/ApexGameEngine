#pragma once

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"
// #include "Apex/Core/Core.h"


namespace Apex {
	
	
	class SceneHeirarchyPanel
	{
	public:
		SceneHeirarchyPanel() = default;
		SceneHeirarchyPanel(const Ref<Scene>& context);
		
		void SetContext(const Ref<Scene>& context);
		void OnImGuiRender();
		
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
		Entity GetSelectedEntity() { return m_SelectedEntity; }
		
	private:
		void DrawEntityNode(Entity entity);
		
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
	
	
}
