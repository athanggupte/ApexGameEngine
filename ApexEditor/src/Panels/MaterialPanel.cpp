#include "apexed_pch.h"
#include "MaterialPanel.h"

#include "Apex/Graphics/Material.h"
#include "Apex/Application.h"

#include <imgui.h>
// ImGui implementations for std::string
#include "CommonResources.h"

#include <misc/cpp/imgui_stdlib.h>

namespace Apex {

	void MaterialPanel::SetContext(Handle materialId)
	{
		m_ContextMaterial = Application::Get().GetResourceManager().Get<Material>(materialId);
	}

	void MaterialPanel::SetContext(const Resource<Material>& materialResource)
	{
		m_ContextMaterial = materialResource;
	}

	Resource<Material> MaterialPanel::GetContext() const
	{
		return m_ContextMaterial;
	}

	void MaterialPanel::OnImGuiRender()
	{
		ImGui::Begin("Material");

		DrawMaterialList();
		if (m_ContextMaterial.IsValid())
			DrawMaterialProperties();

		ImGui::End();
	}

	void MaterialPanel::DrawMaterialList()
	{
		if (ImGui::BeginTable("Materials", 1)) {
			for (auto& [id, ptr] : Application::Get().GetResourceManager().Iterate<Material>()) {
				ImGui::TableNextColumn();
				if (ImGui::Selectable(TO_CSTRING(Strings::Get(id)), (id == m_ContextMaterial.GetId()))) {
					m_ContextMaterial = Application::Get().GetResourceManager().Get<Material>(id);
				}
			}
			ImGui::EndTable();
		}
	}

	void MaterialPanel::DrawMaterialProperties()
	{
		auto materialResName = TO_STRING(Strings::Get(m_ContextMaterial.GetId()));
		if (ImGui::TreeNodeEx((void*)typeid(Material).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, materialResName.c_str())) {
			if (m_ContextMaterial.IsLoaded()) {
				auto shaderResName = TO_STRING(Strings::Get(m_ContextMaterial->GetShader().GetId()));
				// ImGui::InputText("Shader", &shaderResName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginCombo("Shader", shaderResName.c_str())) {
					for (auto& [id, ptr] : Application::Get().GetResourceManager().Iterate<Shader>()) {
						if (ImGui::Selectable(TO_CSTRING(Strings::Get(id)), (id == m_ContextMaterial->GetShader().GetId())))
							m_ContextMaterial->SetShader(Application::Get().GetResourceManager().Get<Shader>(id));
					}
					ImGui::EndCombo();
				}
				for (auto& [name, texture] : m_ContextMaterial->GetTextures()) {
					if (texture.IsValid()) {
						auto textureResName = TO_STRING(Strings::Get(texture.GetId()));
						ImGui::InputText(name.c_str(), &textureResName, ImGuiInputTextFlags_ReadOnly);
						ImGui::Image((void*)(intptr_t)texture->GetID(), Common::GetDefaultImGuiTextureOptions().size,
						             Common::GetDefaultImGuiTextureOptions().uv0, Common::GetDefaultImGuiTextureOptions().uv1, Common::GetDefaultImGuiTextureOptions().tint_col, Common::GetDefaultImGuiTextureOptions().bg_col);
					} else {
						ImGui::InputText(name.c_str(), "None", ImGuiInputTextFlags_ReadOnly);
						ImGui::Image((void*)(intptr_t)Common::GetPlaceholderTexture()->GetID(), Common::GetDefaultImGuiTextureOptions().size,
						             Common::GetDefaultImGuiTextureOptions().uv0, Common::GetDefaultImGuiTextureOptions().uv1, Common::GetDefaultImGuiTextureOptions().tint_col, Common::GetDefaultImGuiTextureOptions().bg_col);
					}
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
							if (const auto filename = static_cast<const char*>(payload->Data); filename != nullptr) {
								const auto id = RESNAME(Utils::GetFilename(filename));
								if (!Application::Get().GetResourceManager().Exists(id)) {
									texture = Application::Get().GetResourceManager().AddResourceFromFile<Texture>(id, filename);
								} else {
									texture = Application::Get().GetResourceManager().Get<Texture>(id);
								}
								Application::Get().GetResourceManager().Load(id);
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
			}
			ImGui::TreePop();
		}
	}
}
