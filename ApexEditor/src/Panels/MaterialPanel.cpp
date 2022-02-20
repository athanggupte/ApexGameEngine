﻿#include "apexed_pch.h"
#include "MaterialPanel.h"

#include "Apex/Graphics/Material.h"
#include "Apex/Application.h"

#include <imgui.h>
// ImGui implementations for std::string
#include "CommonResources.h"
#include "Apex/Graphics/Material.h"
#include "Apex/Graphics/Material.h"
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
		ImGui::Begin("Material", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

		DrawMaterialList();
		if (m_ContextMaterial.IsValid()) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
			if (ImGui::BeginChild("Properties")) {
				ImGui::PopStyleVar();
				DrawMaterialProperties();
			}
			ImGui::EndChild();
		}

		ImGui::End();
	}

	void MaterialPanel::DrawMaterialList()
	{
		if (ImGui::BeginTable("Materials", 1, ImGuiTableFlags_BordersH)) {
			for (auto& [id, ptr] : Application::Get().GetResourceManager().Iterate<Material>()) {
				ImGui::TableNextColumn();
				if (ImGui::Selectable(TO_CSTRING(Strings::Get(id)), (id == m_ContextMaterial.GetId()))) {
					if (id == m_ContextMaterial.GetId())
						m_ContextMaterial = Resource<Material>();
					else
						m_ContextMaterial = Application::Get().GetResourceManager().Get<Material>(id);
				}
			}
			ImGui::EndTable();
		}
	}

	static void DrawAltColorOptions(Material::TextureSlotMap& map);
	static void DrawTextureImageOptions(const std::string& name, Material::TextureSlotMap& map);

	void MaterialPanel::DrawMaterialProperties()
	{
		const auto materialResName = TO_STRING(Strings::Get(m_ContextMaterial.GetId()));
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8.f);
		ON_SCOPE_END { ImGui::PopStyleVar(); };

		if (ImGui::TreeNodeEx((void*)typeid(Material).hash_code(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed, materialResName.c_str())) {
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
					ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.f);
					ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf);
					ImGui::PopStyleVar();

					if (ImGui::BeginTable("Parameters", 2)) {
						ImGui::TableSetupColumn("TextureIcon", ImGuiTableColumnFlags_WidthFixed, Common::GetDefaultImGuiTextureOptions().size.x);
						ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthStretch);
						const std::string nameId = "##" + name;
						auto textureResName = (map.texture.IsValid()) ? TO_STRING(Strings::Get(map.texture.GetId())) : std::string("None");

						// TABLE: Start new row
						ImGui::TableNextRow();
						{ // Set item widths for columns
							ImGui::TableSetColumnIndex(0);
							ImGui::PushItemWidth(Common::GetDefaultImGuiTextureOptions().size.x);
							ImGui::TableSetColumnIndex(1);
							ImGui::PushItemWidth(-FLT_MIN);
						}

						// TABLE: Column 0
						ImGui::TableSetColumnIndex(0);

						ImGui::PushID(name.c_str());
						DrawTextureImageOptions(name, map);
						ImGui::Checkbox("Use", &map.use);

						//TABLE: Column 1
						ImGui::TableSetColumnIndex(1);

						ImGui::InputText(nameId.c_str(), &textureResName, ImGuiInputTextFlags_ReadOnly);
						DrawAltColorOptions(map);

						ImGui::PopID();
					}
					ImGui::EndTable();
				}
			}
			ImGui::TreePop();
		}
	}

	void DrawAltColorOptions(Material::TextureSlotMap& map)
	{
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

	void DrawTextureImageOptions(const std::string& name, Material::TextureSlotMap& map)
	{
		auto textureId = (map.texture.IsValid()) ? map.texture->GetID() : Common::GetPlaceholderTexture()->GetID();

		bool isTextureClicked = ImGui::ImageButton((void*)(intptr_t)textureId,
			                                      Common::GetDefaultImGuiTextureOptions().size,
			                                      Common::GetDefaultImGuiTextureOptions().uv0,
			                                      Common::GetDefaultImGuiTextureOptions().uv1,
			                                      1,
			                                      Common::GetDefaultImGuiTextureOptions().bg_col,
			                                      Common::GetDefaultImGuiTextureOptions().tint_col);
		
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
				if (const auto filename = static_cast<const char*>(payload->Data); filename != nullptr) {
					const auto id = RESNAME(Utils::GetFilename(filename));
					if (!Application::Get().GetResourceManager().Exists(id)) {
						map.texture = Application::Get().GetResourceManager().Insert<Texture>(id, filename);
						map.use = true;
					} else {
						map.texture = Application::Get().GetResourceManager().Get<Texture>(id);
						map.use = true;
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
	}
}
