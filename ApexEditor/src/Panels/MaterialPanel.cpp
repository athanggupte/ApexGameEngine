#include "apexed_pch.h"
#include "MaterialPanel.h"

#include "Apex/Graphics/Material.h"
#include "Apex/Application.h"

#include <imgui.h>
// ImGui implementations for std::string
#include "CommonResources.h"
#include "Apex/Graphics/ShaderDefines.h"
#include "Apex/Utils/ScopeGuard.hpp"

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
				for (auto& [name, map] : m_ContextMaterial->GetTextures()) {
					ImGui::PushID(name.c_str());
					APEX_SCOPE_GUARD { ImGui::PopID(); };

					auto isTextureClicked = false;
					if (map.texture.IsValid()) {
						auto textureResName = TO_STRING(Strings::Get(map.texture.GetId()));
						ImGui::InputText(name.c_str(), &textureResName, ImGuiInputTextFlags_ReadOnly);
						/*ImGui::Image((void*)(intptr_t)map.texture->GetID(),
						             Common::GetDefaultImGuiTextureOptions().size,
						             Common::GetDefaultImGuiTextureOptions().uv0,
						             Common::GetDefaultImGuiTextureOptions().uv1,
						             Common::GetDefaultImGuiTextureOptions().tint_col,
						             Common::GetDefaultImGuiTextureOptions().bg_col);*/
						isTextureClicked = ImGui::ImageButton((void*)(intptr_t)map.texture->GetID(),
						             Common::GetDefaultImGuiTextureOptions().size,
						             Common::GetDefaultImGuiTextureOptions().uv0,
						             Common::GetDefaultImGuiTextureOptions().uv1,
						             1,
						             Common::GetDefaultImGuiTextureOptions().bg_col,
						             Common::GetDefaultImGuiTextureOptions().tint_col);
					} else {
						char tmp[] = "None";
						ImGui::InputText(name.c_str(), tmp, ImGuiInputTextFlags_ReadOnly);
						/*ImGui::Image((void*)(intptr_t)Common::GetPlaceholderTexture()->GetID(),
						             Common::GetDefaultImGuiTextureOptions().size,
						             Common::GetDefaultImGuiTextureOptions().uv0,
						             Common::GetDefaultImGuiTextureOptions().uv1,
						             Common::GetDefaultImGuiTextureOptions().tint_col,
						             Common::GetDefaultImGuiTextureOptions().bg_col);*/
						isTextureClicked = ImGui::ImageButton((void*)(intptr_t)Common::GetPlaceholderTexture()->GetID(),
						             Common::GetDefaultImGuiTextureOptions().size,
						             Common::GetDefaultImGuiTextureOptions().uv0,
						             Common::GetDefaultImGuiTextureOptions().uv1,
						             1,
						             Common::GetDefaultImGuiTextureOptions().bg_col,
						             Common::GetDefaultImGuiTextureOptions().tint_col);
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
							if (const auto filename = static_cast<const char*>(payload->Data); filename != nullptr) {
								const auto id = RESNAME(Utils::GetFilename(filename));
								if (!Application::Get().GetResourceManager().Exists(id)) {
									map.texture = Application::Get().GetResourceManager().AddResourceFromFile<Texture>(id, filename);
								} else {
									map.texture = Application::Get().GetResourceManager().Get<Texture>(id);
								}
								Application::Get().GetResourceManager().Load(id);
							}
						}
						ImGui::EndDragDropTarget();
					}

					if (isTextureClicked) {
						ImGui::OpenPopup("texture-image-menu");
					}

					if (ImGui::BeginPopup("texture-image-menu")) {
						if (ImGui::MenuItem("Remove")) {
							map.texture = Resource<Texture>();
							map.use = false;
						}
						ImGui::EndPopup();
					}

					ImGui::Checkbox("Use Texture", &map.use);

					switch (map.altType)
					{
					case ShaderUniformType::FLOAT:
					{
						if (map.restrictValues)
							ImGui::SliderFloat("##alt-color", &map.altColor.r, 0.f, 1.f);
						else
							ImGui::DragFloat("##alt-color", &map.altColor.r);
						ImGui::Checkbox("Limit", &map.restrictValues);
						break;
					}
					case ShaderUniformType::FLOAT_VEC2:
					{
						if (map.restrictValues)
							ImGui::SliderFloat("##alt-color", &map.altColor.r, 0.f, 1.f);
						else
							ImGui::DragFloat("##alt-color", &map.altColor.r);
						ImGui::Checkbox("Limit", &map.restrictValues);
						break;
					}
					case ShaderUniformType::FLOAT_VEC3:
					{
						ImGuiColorEditFlags flags = (map.restrictValues) ? 0 : ImGuiColorEditFlags_HDR;
						ImGui::ColorEdit3("##alt-color", &map.altColor.r, flags);
						break;
					}
					case ShaderUniformType::FLOAT_VEC4:
					{
						ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ((map.restrictValues) ? 0 : ImGuiColorEditFlags_HDR);
						ImGui::ColorEdit4("##alt-color", &map.altColor.r, flags);
						break;
					}
					default:
						break;
					}
				}
			}
			ImGui::TreePop();
		}
	}

}
