#include <apex_pch.h>
#include "Scene.h"

#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Components.h"

#include "Apex/Graphics/Renderer/Renderer2D.h"
#include "Apex/Physics/PhysicsSystem.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	Scene::Scene()
	{
		//m_Registry.group<TransformComponent, SpriteRendererComponent>();
	}
	
	Entity Scene::CreateEntity(const std::string& name)
	{
		 auto entity = Entity{ m_Registry.create(), this };
		 entity.AddComponent<TransformComponent>();
		 entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		 return entity;
	}

	void Scene::OnUpdate(bool play)
	{
		// Physics Update
		auto view = m_Registry.view<ParticleComponent, TransformComponent>();
		view.each([] (auto& particle, auto& transform) {
			transform.Transform = glm::translate(glm::mat4(1.f), particle.ParticlePtr->Position) * transform.Transform;
		});
		
		// Update Pathfinding
		// Update AI
		// Triggers
		// Sound
		if (play && Options.PrimaryCamera != entt::null) {
			auto [camera, transform] = m_Registry.get<CameraComponent, TransformComponent>(Options.PrimaryCamera);
			Renderer2D::BeginScene(camera.Camera, transform.Transform);
			
			Render2D();
			
			Renderer2D::EndScene();
		}
		// PostProcess
	}
	
	void Scene::Render2D()
	{
		// IMP: When iterating components ownsed by a group outside the group donot add new
		//      entities with the same list of components -> iterators get invalidated
		
		auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		view.each([] (auto& transform, auto& sprite) {
			if (sprite.visible) {
				if (sprite.Texture && sprite.useTexture)
					Renderer2D::DrawQuad(transform, sprite.Texture, sprite.TilingFactor);
				else
					Renderer2D::DrawQuad(transform, sprite.Color);
			}
		});
	}
	
	void Scene::SetPrimaryCamera(const Entity& entity)
	{
		Options.PrimaryCamera = entity.m_EntityId;
	}
	
}
