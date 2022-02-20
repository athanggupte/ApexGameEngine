#include "apexed_pch.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include "Apex/Core/ECS/Components.h"

namespace Apex {
	
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
		
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectedEntity = {};
	}
		
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene");
		
		m_Context->m_Registry.each([&](auto entityId)
		{
			Entity entity{ entityId, m_Context.get() };
			DrawEntityNode(entity);
		});
		
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			m_SelectedEntity = {};

		if (ImGui::BeginPopup("Create")) {
			if (ImGui::MenuItem("Empty Entity")) {
				m_Context->CreateEntity();
			}
			if (ImGui::MenuItem("Camera")) {}
			if (ImGui::MenuItem("Cube")) {}
			if (ImGui::MenuItem("Plane")) {}
			ImGui::EndPopup();
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("Create");
		}

		ImGui::End();
	}
	
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().tag;
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= (m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0;
		// flags |= entity.HasChildren() ? ImGuiTreeNodeFlags_Leaf : 0;

		if (ImGui::BeginPopup("entity-menu-popup")) {
			if (ImGui::MenuItem("Delete")) {
				m_SelectedEntity.Destroy();
				m_SelectedEntity = {};
			}
			ImGui::EndPopup();
		}

		// TODO: Eliminate the need for copying string_view to string
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, std::string(tag.str()).c_str());
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			m_SelectedEntity = entity;
		} else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			m_SelectedEntity = entity;
			ImGui::OpenPopup("entity-menu-popup");
		}
		
		if (opened) {
			// Draw children
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)98141, flags, std::string(tag.str()).c_str());
			if (opened)
				ImGui::TreePop();
			
			ImGui::TreePop();
		}

	}
	
}
