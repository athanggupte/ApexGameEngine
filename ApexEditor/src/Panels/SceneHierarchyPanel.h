#pragma once

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"

namespace Apex {
	
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);
		
		void SetContext(const Ref<Scene>& context);
		
		void OnImGuiRender();
		
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		
	protected:
		void DrawEntityNode(Entity entity);
		
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
	
}
