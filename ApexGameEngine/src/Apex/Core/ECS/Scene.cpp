#include <apex_pch.h>
#include "Scene.h"

#include "Apex/Application.h"
#include "Apex/Core/Camera.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/ScriptableEntity.h"
#include "Apex/Core/ECS/Components.h"

#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/Renderer/Renderer2D.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/Mesh.h"
#include "Apex/Graphics/Material.h"

namespace Apex {

	static Ref<Texture2D> errorTexture;
	static Ref<Shader> errorShader;

	Scene::Scene()
	{
		// (void)m_Registry.group<TransformComponent, SpriteRendererComponent>();
		if (!errorTexture) {
			constexpr auto errorTextureSpec = TextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE };
			errorTexture = Texture2D::Create(1U, 1U, errorTextureSpec, "error");
			uint32_t errorTextureData = 0xfff933f2;
			errorTexture->SetData(&errorTextureData, sizeof(errorTextureData));
		}
		if (!errorShader) {
			constexpr auto vertexSource = R"(
			#version 450 core

			#include <internal_assets/shaders/ShaderDefines.h>

			layout(location = ATTRIB_LOC_Position) in vec3 a_Position;

			layout(location = 0) uniform Camera {
				vec3 u_CameraPosition;
				mat4 u_ViewProjection;
			};
			layout(location = 1) uniform mat4 u_Model;

			layout(location = 0) out vec3 v_Position;

			void main() {
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
			)";

			constexpr auto fragmentSource = R"(
			#version 450 core

			layout(location = 0) out vec4 o_Color;

			layout(location = 0) in vec3 v_Position;

			void main() {
				o_Color = vec4(0.976, 0.2, 0.949, 1.0);
			}
			)";

			errorShader = Shader::Create("ErrorShader", vertexSource, fragmentSource);
		}
	}
	
	Entity Scene::CreateEntity(StringHandle name)
	{
		auto entity = Entity{ m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}

	Entity Scene::CreateEntity()
	{
		// static uint32_t counter = 1;
		return CreateEntity(HASH("Unnamed Entity"));
	}

	void Scene::OnSetup()
	{
		auto& resourceManager = Application::Get().GetResourceManager();

		m_Registry.view<SpriteRendererComponent>()
			.each([&resourceManager](SpriteRendererComponent& sprite) {
				if (sprite.texture.IsValid())
					resourceManager.Load(sprite.texture.GetId());
			});

		m_Registry.view<MeshRendererComponent>()
			.each([&resourceManager](MeshRendererComponent& meshComp) {
				if (meshComp.mesh.IsValid())
					resourceManager.Load(meshComp.mesh.GetId());
				if (meshComp.material.IsValid())
					resourceManager.Load(meshComp.material.GetId());
			});
	}

	void Scene::Render2D()
	{
		// IMP: When iterating components owned by a group outside the group donot add new
		//      entities with the same list of components -> iterators get invalidated

		const auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		view.each([](const TransformComponent& transform, const SpriteRendererComponent& sprite) {
			if (sprite.visible) {
				if (sprite.useTexture) {
					if (sprite.texture.IsValid() && sprite.texture.IsLoaded()) {
						Renderer2D::DrawQuad(transform.GetTransform(), std::dynamic_pointer_cast<Texture2D>(sprite.texture.Get()), sprite.tilingFactor);
					} else {
						Renderer2D::DrawQuad(transform.GetTransform(), { 0.95f, 0.2f, 0.98f, 1.0f });
					}
				}
				else
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
			}
		});
	}

	void Scene::Render3D()
	{
		errorTexture->Bind(0);
		const auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		view.each([](const TransformComponent& transform, MeshRendererComponent& mesh_component) {
			if (mesh_component.mesh.IsValid() && mesh_component.material.IsValid()) {
				if (const auto shader = mesh_component.material->GetShader(); shader.IsValid()) {
					int slot = 1;
					for (auto& [name, texture] : mesh_component.material->GetTextures()) {
						if (texture.IsValid()) {
							texture->Bind(++slot);
							shader->SetUniInt1(name, slot);
						} else {
							shader->SetUniInt1(name, 0);
						}
					}
					Renderer::Submit(shader.Get(), mesh_component.mesh->GetVAO(), transform.GetTransform());
				} else {
					Renderer::Submit(errorShader, mesh_component.mesh->GetVAO(), transform.GetTransform());
				}
			} else {
				Renderer::Submit(errorShader, mesh_component.mesh->GetVAO(), transform.GetTransform());
			}
		});
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Physics Update
		// Update Pathfinding
		// Update AI
		// Triggers
		// Sound
		// Render
		if (Options.PrimaryCamera != entt::null) {
			auto [camera, transform] = m_Registry.get<CameraComponent, TransformComponent>(Options.PrimaryCamera);
			Renderer2D::BeginScene(camera.camera, transform.GetTransform());
			Render2D();
			Renderer2D::EndScene();
		}
		// PostProcess
	}

	void Scene::OnEditorUpdate(Timestep ts)
	{
		Render3D();
		Render2D();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		Options.ViewportWidth = width;
		Options.ViewportHeight = height;
		
		m_Registry.view<CameraComponent>().each([this](auto& cameraComponent) {
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.SetViewportSize(Options.ViewportWidth, Options.ViewportHeight);
		});
	}
	
	void Scene::SetPrimaryCamera(const Entity& entity)
	{
		Options.PrimaryCamera = entity.m_EntityId;
	}

	template<typename Component_t>
	void Scene::OnComponentAdded(Entity entity, Component_t& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, TagComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, TransformComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, CameraComponent& component)
	{
		if (Options.ViewportWidth > 0 && Options.ViewportHeight > 0)
			component.camera.SetViewportSize(Options.ViewportWidth, Options.ViewportHeight);
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, SpriteRendererComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded(Entity entity, MeshRendererComponent& component)
	{
	}
	
}
