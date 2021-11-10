#include <apex_pch.h>
#include "Scene.h"

#include "Apex/Application.h"
#include "Apex/Core/Camera.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/ScriptableEntity.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/Renderer/Renderer2D.h"

namespace Apex {

	Scene::Scene()
	{
		// (void)m_Registry.group<TransformComponent, SpriteRendererComponent>();
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
		const auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		view.each([](const TransformComponent& transform, const MeshRendererComponent& mesh_component) {
			if (mesh_component.mesh.IsValid() && mesh_component.material.IsValid()) {
				if (const auto& shader = mesh_component.material->GetShader()) {
					mesh_component.material->Bind();
					Renderer::Submit(shader, mesh_component.mesh->GetVAO(), transform.GetTransform());
				}
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
