#include <apex_pch.h>
#include "Scene.h"

#include "Apex/Application.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/ScriptableEntity.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Core/ECS/Components/SceneCamera.h"
#include "Apex/Graphics/Renderer/Renderer2D.h"

namespace Apex {

	Scene::Scene()
	{
		(void)m_Registry.group<TransformComponent, SpriteRendererComponent>();
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
				if (std::holds_alternative<Handle>(sprite.Texture)) {
					auto handle = std::get<Handle>(sprite.Texture);
					auto textureResource = resourceManager.Get(handle);
					if (!textureResource) {
						APEX_CORE_ERROR("Texture resource {} not found!", Strings::Get(std::get<Handle>(sprite.Texture)));
						sprite.Texture = (Resource*)nullptr;
					}
					else {
						sprite.Texture = textureResource;
					}
				}
				if (std::get<Resource*>(sprite.Texture) != nullptr)
					std::get<Resource*>(sprite.Texture)->Load();
			});
	}

	void Scene::DrawSprites()
	{
		// IMP: When iterating components owned by a group outside the group donot add new
		//      entities with the same list of components -> iterators get invalidated

		auto group = m_Registry.group<TransformComponent, SpriteRendererComponent>();
		group.each([](TransformComponent& transform, SpriteRendererComponent& sprite) {
			if (sprite.visible) {
				auto texture = std::get<Resource*>(sprite.Texture);
				if (sprite.useTexture && texture)
					Renderer2D::DrawQuad(transform.GetTransform(), std::dynamic_pointer_cast<Texture2D>(texture->Get<Texture>()), sprite.TilingFactor);
				else
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
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
			Renderer2D::BeginScene(camera.Camera, transform.GetTransform());
			DrawSprites();
			Renderer2D::EndScene();
		}
		// PostProcess
	}
	
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		Options.ViewportWidth = width;
		Options.ViewportHeight = height;
		
		m_Registry.view<CameraComponent>().each([this](auto& cameraComponent) {
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(Options.ViewportWidth, Options.ViewportHeight);
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
			component.Camera.SetViewportSize(Options.ViewportWidth, Options.ViewportHeight);
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, SpriteRendererComponent& component)
	{
	}
	
	template<>
	void Scene::OnComponentAdded(Entity entity, ScriptComponent& component)
	{
	}
	
}
