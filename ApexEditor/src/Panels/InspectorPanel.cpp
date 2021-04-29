#include "InspectorPanel.h"
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#ifdef __STDC_LIB_EXT1__
#define STRCPY(dest, size, src) strcpy_s(dest, size, src)
#else
#warning Using unsafe `strcpy` as `strcpy_s` not available.
#define STRCPY(dest, size, src) strcpy(dest, src)
#endif

#include <imgui.h>

#include "Apex/Core/ECS/Components.h"

namespace Apex {
	
	InspectorPanel::InspectorPanel(Entity entity, const Ref<Scene>& scene)
	{
		SetContext(entity, scene);
	}
		
	void InspectorPanel::SetContext(Entity entity, const Ref<Scene>& scene)
	{
		m_ContextEntity = entity;
		m_ContextScene = scene;
	}
		
	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin("Inspector");
		
		if (m_ContextEntity) {
			DrawComponents();
		}
		
		ImGui::End();
	}
	
	void InspectorPanel::DrawComponents()
	{
		// Tag Component
		{
			auto& tag = m_ContextEntity.GetComponent<TagComponent>().Tag;
				
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			STRCPY(buffer, sizeof(buffer), tag.c_str());
			
			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}
		
		// Transform Component
		if (m_ContextEntity.HasComponent<TransformComponent>()) {
			
		}
	}
	
}
