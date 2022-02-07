#include <apexed_pch.h>
#include "InspectorPanel.h"
#include <cstring>
#define STRCPY(dest, size, src) strcpy_s(dest, size, src)

#include "Apex/Application.h"

#include "Apex/Core/ECS/Components.h"
#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Utils/ScopeGuard.hpp"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/Mesh.h"
#include "Apex/Graphics/Material.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

// ImGui implementations for std::string
#include "CommonResources.h"



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
			DrawAddComponentsMenu();
		}
		
		ImGui::End();
	}
	
	static char changedBuf[256]{ 0 };


	// TODO: Remove from here and add to custom ui file
	static bool ButtonEx_custom(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
	{
	    ImGuiWindow* window = ImGui::GetCurrentWindow();
	    if (window->SkipItems)
	        return false;

	    ImGuiContext& g = *GImGui;
	    const ImGuiStyle& style = g.Style;
	    const ImGuiID id = window->GetID(label);
	    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	    ImVec2 pos = window->DC.CursorPos;
	    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
	        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	    const ImRect bb(pos, pos + size);
	    ImGui::ItemSize(size, style.FramePadding.y);
	    if (!ImGui::ItemAdd(bb, id))
	        return false;

	    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
	        flags |= ImGuiButtonFlags_Repeat;

	    bool hovered, held;
	    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	    // Render
	    const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	    ImGui::RenderNavHighlight(bb, id);
	    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		const ImU32 textCol = ImGui::GetColorU32((held && hovered) ? ImGuiCol_Button : hovered ? ImGuiCol_Text : ImGuiCol_Text);
		ImGui::PushStyleColor(ImGuiCol_Text, textCol);

	    if (g.LogEnabled)
		    ImGui::LogSetNextTextDecoration("[", "]");
	    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		ImGui::PopStyleColor();

	    // Automatically close popups
	    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	    //    CloseCurrentPopup();

	    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	    return pressed;
	}


	void InspectorPanel::DrawComponents()
	{
		constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | 
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing,  0.f);
		APEX_SCOPE_GUARD
		{
			ImGui::PopStyleVar(1);
		};

		// GUID
		auto& id = m_ContextEntity.GetComponent<IDComponent>().id;
		ImGui::TextColored({ 0.6f, 0.6f, 0.6f, 1.f }, TO_CSTRING(fmt::format("({})", Strings::Get(id.Hash()))));

		// Tag Component
		{
			auto& tag = m_ContextEntity.GetComponent<TagComponent>().tag;
				
			static char buffer[129] = {};
			static constexpr size_t BUFFER_SIZE = sizeof(buffer) - 1;
			auto sz = tag.str().size();
			memcpy_s(buffer, BUFFER_SIZE, tag.str().data(), tag.str().size());
			const auto last = (sz < BUFFER_SIZE) ? sz : BUFFER_SIZE;
			buffer[last] = 0;
			
			static bool isChanged = false;

			if (ImGui::InputText("Tag", buffer, BUFFER_SIZE)) {
				isChanged = true;
				strcpy_s(changedBuf, sizeof(changedBuf), buffer);
			}

			if (isChanged) {
				ImGui::SameLine();
				if (ImGui::Button("OK")) {
					tag = HASH(changedBuf);
					isChanged = false;
				}
			}

			ImGui::Separator();
		}
		
		// Transform Component
		{
			auto& transformComp = m_ContextEntity.GetComponent<TransformComponent>();

			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform")) {
				auto dragSpeed = (Input::IsKeyPressed(APEX_KEY_LEFT_SHIFT)) ? 0.001f : 0.5f;
					
				ImGui::DragFloat3("Translation", glm::value_ptr(transformComp.translation), dragSpeed);
				ImGui::DragFloat3("Rotation", glm::value_ptr(transformComp.rotation), dragSpeed);
				ImGui::DragFloat3("Scale", glm::value_ptr(transformComp.scale), dragSpeed);
				ImGui::TreePop();
			}
			ImGui::Separator();
		}

		// Camera Component
		if (m_ContextEntity.HasComponent<CameraComponent>()) {
			auto& cameraComp = m_ContextEntity.GetComponent<CameraComponent>();
			auto& camera = cameraComp.camera;

			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera")) {
				constexpr const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				constexpr const size_t projectionTypesLen = std::size(projectionTypeStrings);
				const char* currentProjectionTypeStr = projectionTypeStrings[(int)cameraComp.camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeStr)) {
					for (int i=0; i<projectionTypesLen; i++) {
						bool isSelected = projectionTypeStrings[i] == currentProjectionTypeStr;
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
							currentProjectionTypeStr = projectionTypeStrings[i];
							camera.SetProjectionType(static_cast<Camera::ProjectionType>(i));
						}
						
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					
					ImGui::EndCombo();
				}
				
				if (cameraComp.camera.GetProjectionType() == Camera::ProjectionType::Perspective) {
					float perspFov = glm::degrees(camera.GetPerspectiveVerticalFov());
					if (ImGui::DragFloat("Vertical FOV", &perspFov))
						camera.SetPerspectiveVerticalFov(glm::radians(perspFov));
					
					float perspNear = camera.GetPerspectiveNear();
					if (ImGui::DragFloat("Near", &perspNear))
						camera.SetPerspectiveNear(perspNear);
					
					float perspFar = camera.GetPerspectiveFar();
					if (ImGui::DragFloat("Far", &perspFar))
						camera.SetPerspectiveFar(perspFar);
				}
				else {
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);
					
					float orthoNear = camera.GetOrthographicNear();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNear(orthoNear);
					
					float orthoFar = camera.GetOrthographicFar();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFar(orthoFar);
				}

				if (ImGui::Button("Delete Component")) {
					m_ContextEntity.RemoveComponent<CameraComponent>();
				}

				ImGui::TreePop();
			}
		}

		// Sprite Renderer Component
		if (m_ContextEntity.HasComponent<SpriteRendererComponent>()) {
			auto& sprite = m_ContextEntity.GetComponent<SpriteRendererComponent>();

			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer")) {
				ImGui::Checkbox("Visible", &sprite.visible);
				// Display component members
				ImGui::Checkbox("Use Texture", &sprite.useTexture);
				if (sprite.useTexture) {
					if (sprite.texture.IsValid() && sprite.texture.IsLoaded()) {
						auto textureResName = TO_STRING(Strings::Get(sprite.texture.GetId()));
						ImGui::InputText("Texture", textureResName.data(), textureResName.size(), ImGuiInputTextFlags_ReadOnly);
						ImGui::Image((void*)(intptr_t)sprite.texture->GetID(), Common::GetDefaultImGuiTextureOptions().size,
							Common::GetDefaultImGuiTextureOptions().uv0, Common::GetDefaultImGuiTextureOptions().uv1, Common::GetDefaultImGuiTextureOptions().tint_col, Common::GetDefaultImGuiTextureOptions().bg_col);
					} else {
						ImGui::InputText("Texture", "No texture", sizeof("No texture"), ImGuiInputTextFlags_ReadOnly);
						ImGui::Image((void*)(intptr_t)Common::GetPlaceholderTexture()->GetID(), Common::GetDefaultImGuiTextureOptions().size,
							Common::GetDefaultImGuiTextureOptions().uv0, Common::GetDefaultImGuiTextureOptions().uv1, Common::GetDefaultImGuiTextureOptions().tint_col, Common::GetDefaultImGuiTextureOptions().bg_col);
					}
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
							if (const auto filename = static_cast<const char*>(payload->Data); filename != nullptr) {
								const auto id = RESNAME(Utils::GetFilename(filename));
								if (!Application::Get().GetResourceManager().Exists(id)) {
									sprite.texture = Application::Get().GetResourceManager().AddResourceFromFile<Texture>(id, filename);
								} else {
									sprite.texture = Application::Get().GetResourceManager().Get<Texture>(id);
								}
								Application::Get().GetResourceManager().Load(id);
							}
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::DragFloat("Tiling Factor", &sprite.tilingFactor);
				} else {
					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
				}

				if (ImGui::Button("Delete Component")) {
					m_ContextEntity.RemoveComponent<SpriteRendererComponent>();
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
		}

		// Mesh Renderer Component
		if (m_ContextEntity.HasComponent<MeshRendererComponent>()) {
			auto& meshComp = m_ContextEntity.GetComponent<MeshRendererComponent>();

			if (ImGui::TreeNodeEx((void*)typeid(MeshRendererComponent).hash_code(), treeNodeFlags, "Mesh Renderer")) {
				if (meshComp.mesh.IsValid()) {
					auto meshResName = TO_STRING(Strings::Get(meshComp.mesh.GetId()));

					// ImGui::InputText("Mesh", &meshResName, ImGuiInputTextFlags_ReadOnly);
					if (ImGui::BeginCombo("Mesh", meshResName.c_str())) {
						for (auto& [id, ptr] : Application::Get().GetResourceManager().Iterate<Mesh>()) {
							if (ImGui::Selectable(TO_CSTRING(Strings::Get(id)), (id == meshComp.mesh.GetId()))) {
								meshComp.mesh = Application::Get().GetResourceManager().Get<Mesh>(id);
							}
						}
						ImGui::EndCombo();
					}
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
							if (const auto filename = static_cast<const char*>(payload->Data); filename != nullptr) {
								const auto id = RESNAME(Utils::GetFilename(filename));
								if (!Application::Get().GetResourceManager().Exists(id)) {
									meshComp.mesh = Application::Get().GetResourceManager().AddResourceFromFile<Mesh>(id, filename);
								} else {
									meshComp.mesh = Application::Get().GetResourceManager().Get<Mesh>(id);
								}
								Application::Get().GetResourceManager().Load(id);
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
				std::string materialResName;
				if (meshComp.material.IsValid()) {
					materialResName = TO_STRING(Strings::Get(meshComp.material.GetId()));
				}
				if (ImGui::BeginCombo("Material", materialResName.c_str())) {
					for (auto& [id, ptr] : Application::Get().GetResourceManager().Iterate<Material>()) {
						if (ImGui::Selectable(TO_CSTRING(Strings::Get(id)), (id == meshComp.material.GetId())))
							meshComp.material = Application::Get().GetResourceManager().Get<Material>(id);
					}
					ImGui::EndCombo();
				}

				auto textCol = ImGui::GetStyleColorVec4(ImGuiCol_Text);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.941f, 0.176f, 0.121f, 1.f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.835f, 0.133f, 0.082f, 1.f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, textCol);
				if (ButtonEx_custom("Delete Component", { ImGui::GetContentRegionAvail().x, 0.f }, ImGuiButtonFlags_None)) {
					m_ContextEntity.RemoveComponent<MeshRendererComponent>();
				}
				ImGui::PopStyleColor(3);
				ImGui::TreePop();
			}
			ImGui::Separator();
		}

		if (m_ContextEntity.HasComponent<NativeScriptComponent>()) {
			auto& scriptComp = m_ContextEntity.GetComponent<NativeScriptComponent>();
			const std::string scriptName = TO_STRING(Strings::Get(scriptComp.factory.GetId()));
			if (ImGui::TreeNodeEx((void*)typeid(NativeScriptComponent).hash_code(), treeNodeFlags, scriptName.c_str())) {
				if (ImGui::BeginTable("Parameters", 2, ImGuiTableFlags_BordersInner)) {
					for (auto i = 0; i < 5; i++) {
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Param_%d", i+1);
						ImGui::TableNextColumn();
						ImGui::Text("some value");
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
	}

	void InspectorPanel::DrawAddComponentsMenu()
	{
		if (ImGui::BeginPopup("Add Component")) {
			if (!m_ContextEntity.HasComponent<CameraComponent>())
				if (ImGui::MenuItem("Camera"))
					m_ContextEntity.AddComponent<CameraComponent>();
			
			if (!m_ContextEntity.HasComponent<SpriteRendererComponent>())
				if (ImGui::MenuItem("Sprite Renderer"))
					m_ContextEntity.AddComponent<SpriteRendererComponent>();

			if (!m_ContextEntity.HasComponent<MeshRendererComponent>())
				if (ImGui::MenuItem("Mesh Renderer")) {
					auto& meshComp = m_ContextEntity.AddComponent<MeshRendererComponent>();
					meshComp.mesh = Application::Get().GetResourceManager().Get<Mesh>(RESNAME("default-cube"));
					//meshComp.material = Application::Get().GetResourceManager().Get<Material>(RESNAME("material_Unlit"));
				}
						
			if (ImGui::BeginMenu("Physics")) {
				
				// if (!m_ContextEntity.HasComponent<Collider3DComponent>())
					if (ImGui::MenuItem("Collider 3D")) {}
				
				// if (!m_ContextEntity.HasComponent<Collider2DComponent>())
					if (ImGui::MenuItem("Collider 2D")) {}
				
				// if (!m_ContextEntity.HasComponent<RigidBody3DComponent>())
					if (ImGui::MenuItem("Rigid Body 3D")) {}
				
				// if (!m_ContextEntity.HasComponent<RigidBody2DComponent>())
					if (ImGui::MenuItem("Rigid Body 2D")) {}
				
				ImGui::EndMenu();
			}

			if (!m_ContextEntity.HasComponent<NativeScriptComponent>()) {
				if (ImGui::BeginMenu("NativeScriptComponent")) {
					for (auto& scriptRes : Application::Get().GetResourceManager().Iterate<AScriptFactory>()) {
						auto& [id, ptr] = scriptRes;
						if (ImGui::MenuItem(TO_CSTRING(Strings::Get(id)))) {
							auto& nsc = m_ContextEntity.AddComponent<NativeScriptComponent>(scriptRes.ToResource());
						}
					}
					ImGui::EndMenu();
				}
			}

			ImGui::EndPopup();
		}
		
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");
	}
}
