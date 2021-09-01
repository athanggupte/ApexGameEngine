#include <apexed_pch.h>
#include "InspectorPanel.h"
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#ifdef __STDC_LIB_EXT1__
#define STRCPY(dest, size, src) strcpy_s(dest, size, src)
#else
//#warning Using unsafe `strcpy` as `strcpy_s` not available.
#pragma message "Using unsafe `strcpy` as `strcpy_s` not available."
#define STRCPY(dest, size, src) strcpy(dest, src)
#endif

#include "Apex/Core/ECS/Components.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Utils/Utils.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui implementations for std::string
#include <misc/cpp/imgui_stdlib.cpp>

namespace Apex {
	
	static Ref<Texture2D> s_PlaceholderTexture;
	
	InspectorPanel::InspectorPanel(Entity entity, const Ref<Scene>& scene)
	{
		SetContext(entity, scene);
	}
		
	void InspectorPanel::SetContext(Entity entity, const Ref<Scene>& scene)
	{
		m_ContextEntity = entity;
		m_ContextScene = scene;
		
		if (!s_PlaceholderTexture) {
			s_PlaceholderTexture = Texture2D::Create(2, 2, SimpleTextureSpec, "PlaceholderTexture");
			uint32_t texData[4] = { 0xaaaaaaaa, 0xdddddddd, 0xdddddddd, 0xaaaaaaaa };
			s_PlaceholderTexture->SetData(texData, sizeof(texData));
		}
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
		ImGui::Separator();
		
		// Transform Component
		if (m_ContextEntity.HasComponent<TransformComponent>()) {
			auto& transformComp = m_ContextEntity.GetComponent<TransformComponent>();
			
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
			
			bool opened = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), flags, "Transform");
			
			if (opened) {
				auto dragSpeed = (Input::IsKeyPressed(APEX_KEY_LEFT_SHIFT)) ? 0.001f : 0.5f;
					
				ImGui::DragFloat3("Translation", glm::value_ptr(transformComp.Translation), dragSpeed);
				ImGui::DragFloat3("Rotation", glm::value_ptr(transformComp.Rotation), dragSpeed);
				ImGui::DragFloat3("Scale", glm::value_ptr(transformComp.Scale), dragSpeed);
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
		
		if (m_ContextEntity.HasComponent<CameraComponent>()) {
			auto& cameraComp = m_ContextEntity.GetComponent<CameraComponent>();
			auto& camera = cameraComp.Camera;
			
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
			
			bool opened = ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), flags, "Camera");
			
			if (opened) {
				constexpr const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				constexpr const size_t projectionTypesLen = sizeof(projectionTypeStrings) / sizeof(projectionTypeStrings[0]);
				const char* currentProjectionTypeStr = projectionTypeStrings[(int)cameraComp.Camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeStr)) {
					for (int i=0; i<projectionTypesLen; i++) {
						bool isSelected = projectionTypeStrings[i] == currentProjectionTypeStr;
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
							currentProjectionTypeStr = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}
						
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					
					ImGui::EndCombo();
				}
				
				if (cameraComp.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
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
				
				ImGui::TreePop();
			}
		}
		
		if (m_ContextEntity.HasComponent<SpriteRendererComponent>()) {
			auto& sprite = m_ContextEntity.GetComponent<SpriteRendererComponent>();
			
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
			
			bool opened = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), flags, "Sprite");
			
			if (opened) {
				// Display component members
				ImGui::Checkbox("Use Texture", &sprite.useTexture);
				if (sprite.useTexture) {
					ImGui::TextUnformatted("Texture");
					ImGui::SameLine();
					
					ImVec2 size = ImVec2(100.0f, 100.0f);                // Image Size
					int frame_padding = -1;                              // -1 == uses default padding (style.FramePadding)
					ImVec2 uv0 = ImVec2(0.0f, 0.0f);                     // UV coordinates for lower-left
					ImVec2 uv1 = ImVec2(1.0f, 1.0f);                     // UV coordinates for (32,32) in our texture
					ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // Black background
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // No tint
					if (sprite.Texture) {
						// ImGui::Text("%s", sprite.Texture.GetGUID().GetString().c_str());
						if (ImGui::ImageButton((void*)(intptr_t)sprite.Texture->GetID(), size, uv0, uv1, frame_padding, bg_col, tint_col)) {
							auto filename = Utils::OpenFileDialog();
							if (!filename.empty())
								sprite.Texture = Texture2D::Create(filename);
						}
					} else {
						ImGui::TextUnformatted("No image");
						if (ImGui::ImageButton((void*)(intptr_t)s_PlaceholderTexture->GetID(), size, uv0, uv1, frame_padding, bg_col, tint_col)) {
							auto filename = Utils::OpenFileDialog();
							if (!filename.empty())
								sprite.Texture = Texture2D::Create(filename);
						}
					}
					ImGui::DragFloat("Tiling Factor", &sprite.TilingFactor);
				} else {
					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
		
		if (m_ContextEntity.HasComponent<ScriptComponent>()) {
			auto& scriptComp = m_ContextEntity.GetComponent<ScriptComponent>();
			
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
			
			bool opened = ImGui::TreeNodeEx((void*)typeid(ScriptComponent).hash_code(), flags, "Script");
			
			if (opened) {
				ImGui::InputText("Script File", &scriptComp.Filename);
				ImGui::SameLine();
				if (ImGui::Button("...##script_file_select")) {
					scriptComp.Filename = Utils::OpenFileDialog();
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
		
		if (ImGui::BeginPopup("Add Component")) {
			if (!m_ContextEntity.HasComponent<CameraComponent>())
				if (ImGui::MenuItem("Camera"))
					m_ContextEntity.AddComponent<CameraComponent>();
			
			if (!m_ContextEntity.HasComponent<SpriteRendererComponent>())
				if (ImGui::MenuItem("Sprite"))
					m_ContextEntity.AddComponent<SpriteRendererComponent>();
			
			if (!m_ContextEntity.HasComponent<ScriptComponent>())
				if (ImGui::MenuItem("Script"))
					m_ContextEntity.AddComponent<ScriptComponent>();
			
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
			ImGui::EndPopup();
		}
		
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");
		
		
	}
	
}
