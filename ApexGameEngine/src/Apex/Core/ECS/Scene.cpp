#include <apex_pch.h>
#include "Scene.h"

#include "Entity.hpp"
#include "Components.h"

#include "Apex/Graphics/Renderer/Renderer2D.h"

namespace Apex {

	Entity Scene::CreateEntity(const std::string& name)
	{
		 auto entity = Entity{ m_Registry.create(), this };
		 entity.AddComponent<TransformComponent>();
		 entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		 return entity;
	}

	void Scene::OnUpdate()
	{
		// Physics Update
		// Update Pathfinding
		// Update AI
		// Triggers
		// Sound
		Render2D();
		// PostProcess
	}
	
	void Scene::Render2D()
	{
		auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
		
		view.each([] (auto& transform, auto& sprite) {
			Renderer2D::DrawQuad(transform, sprite.Color);
		});
	}
	
}
