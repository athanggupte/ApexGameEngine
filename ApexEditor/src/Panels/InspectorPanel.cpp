#include <apexed_pch.h>
#include "InspectorPanel.h"
#include <cstring>
#define STRCPY(dest, size, src) strcpy_s(dest, size, src)

#include "Apex/Application.h"

#include "Apex/Core/ECS/Components.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Core/Input/Input.h"
#include "Apex/Core/Input/KeyCodes.h"
#include "Apex/Utils/Utils.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui implementations for std::string
#include "UI/ImGuiCustom.h"

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
			DrawAddComponentsMenu();
		}
		
		ImGui::End();
	}
	
	static char changedBuf[256]{ 0 };

	template<typename Resource_t>
	static void LoadResource(Handle handle, const char* filename)
	{
		
	}

	void InspectorPanel::DrawComponents()
	{
		constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | 
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

		// Tag Component
		{
			auto& tag = m_ContextEntity.GetComponent<TagComponent>().tag;
				
			static char buffer[256] = {};
			auto sz = tag.str().size();
			memcpy_s(buffer, sizeof(buffer), tag.str().data(), tag.str().size());
			
			static bool isChanged = false;

			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
				isChanged = true;
				strcpy_s(changedBuf, sizeof(changedBuf), buffer);
			}

			if (isChanged) {
				ImGui::SameLine();
				if (Apex::Button("OK")) {
					tag = HASH(changedBuf);
					isChanged = false;
				}
			}

			ImGui::Separator();
		}
		
		// Transform Component
		{
			auto& transformComp = m_ContextEntity.GetComponent<TransformComponent>();

			bool opened = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");
			
			if (opened) {
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

				if (Apex::Button("Delete Component")) {
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
					const ImVec2 size = ImVec2(100.0f, 100.0f);                // Image Size
					const int frame_padding = -1;                              // -1 == uses default padding (style.FramePadding)
					const ImVec2 uv0 = ImVec2(0.0f, 0.0f);                     // UV coordinates for lower-left
					const ImVec2 uv1 = ImVec2(1.0f, 1.0f);                     // UV coordinates for (32,32) in our texture
					const ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // Black background
					const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // No tint
					if (sprite.texture.IsValid() && sprite.texture.IsLoaded()) {
						ImGui::InputText("Texture", &TO_STRING(Strings::Get(sprite.texture.GetId())), ImGuiInputTextFlags_ReadOnly);
						ImGui::Image((void*)(intptr_t)sprite.texture->GetID(), size, uv0, uv1, tint_col, bg_col);
					} else {
						ImGui::InputText("Texture", "No texture", sizeof("No texture"), ImGuiInputTextFlags_ReadOnly);
						ImGui::Image((void*)(intptr_t)s_PlaceholderTexture->GetID(), size, uv0, uv1, tint_col, bg_col);
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

				if (Apex::Button("Delete Component")) {
					m_ContextEntity.RemoveComponent<SpriteRendererComponent>();
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
		}

		// Mesh Renderer Component
		if (m_ContextEntity.HasComponent<MeshRendererComponent>()) {
			auto& meshComp = m_ContextEntity.GetComponent<MeshRendererComponent>();

			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Mesh Renderer")) {
				if (meshComp.mesh.IsValid()) {
					auto meshResName = TO_STRING(Strings::Get(meshComp.mesh.GetId()));

					ImGui::InputText("Mesh", &meshResName, ImGuiInputTextFlags_ReadOnly);
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
				if (meshComp.material.IsValid()) {
					auto materialResName = TO_STRING(Strings::Get(meshComp.material.GetId()));
					if (ImGui::TreeNodeEx((void*)typeid(Material).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Material")) {
						ImGui::InputText("Name", &materialResName, ImGuiInputTextFlags_ReadOnly);
						if (meshComp.material.IsLoaded()) {
							for (auto& [name, texture] : meshComp.material->GetTextures()) {
								const ImVec2 size = ImVec2(100.0f, 100.0f);                // Image Size
								const int frame_padding = -1;                                    // -1 == uses default padding (style.FramePadding)
								const ImVec2 uv0 = ImVec2(0.0f, 0.0f);                     // UV coordinates for lower-left
								const ImVec2 uv1 = ImVec2(1.0f, 1.0f);                     // UV coordinates for (32,32) in our texture
								const ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // Black background
								const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // No tint
								ImGui::InputText("Texture", &Utils::GetFilename(texture->GetPath()), ImGuiInputTextFlags_ReadOnly);
								ImGui::Image((void*)(intptr_t)texture->GetID(), size, uv0, uv1, tint_col, bg_col);

								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
										if (const auto filename = static_cast<const char*>(payload->Data); filename != nullptr) {
											// meshComp.material = RESNAME(Utils::GetFilename(filename));
											// LoadResource<Material>(meshComp.material, filename);
											auto handle = RESNAME(Utils::GetFilename(filename));
											if (!Application::Get().GetResourceManager().Exists(handle)) {
												Application::Get().GetResourceManager().AddResourceFromFile<Texture>(handle, filename);
											}
											if (!Application::Get().GetResourceManager().Get<Texture>(handle).IsLoaded())
												Application::Get().GetResourceManager().Load(handle);
											texture = Application::Get().GetResourceManager().Get<Texture>(RESNAME(Utils::GetFilename(filename))).Get();
										}
									}
									ImGui::EndDragDropTarget();
								}
							}
						}
						ImGui::TreePop();
					}
				}

				if (Apex::Button("Delete Component")) {
					m_ContextEntity.RemoveComponent<MeshRendererComponent>();
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
		}

		// Script Component
		if (m_ContextEntity.HasComponent<ScriptComponent>()) {
			auto& scriptComp = m_ContextEntity.GetComponent<ScriptComponent>();
			
			bool opened = ImGui::TreeNodeEx((void*)typeid(ScriptComponent).hash_code(), treeNodeFlags, "Script");
			
			if (opened) {
				ImGui::InputText("Script File", &scriptComp.filename);
				ImGui::SameLine();
				if (ImGui::Button("...##script_file_select")) {
					scriptComp.filename = Utils::OpenFileDialog();
				}
				ImGui::TreePop();
			}
			ImGui::Separator();
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
					meshComp.material = Application::Get().GetResourceManager().Get<Material>(RESNAME("material_Unlit"));
				}

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
		
		if (Apex::Button("Add Component"))
			ImGui::OpenPopup("Add Component");
	}
}
