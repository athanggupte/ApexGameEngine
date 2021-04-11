#pragma once

#include <glm/glm.hpp>

#define COMPONENT_DEFAULT_CTR(name)	name() = default;\
									name(const name&) = default

#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {

	struct TagComponent
	{
		std::string Tag;
		
		COMPONENT_DEFAULT_CTR(TagComponent);
		
		TagComponent(const std::string& tag)
			: Tag(tag) {}
		
		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};
	
	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4{ 1.f };
		
		//TransformComponent() = default;
		//TransformComponent(const TransformComponent&) = default;
		COMPONENT_DEFAULT_CTR(TransformComponent);
		
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}
		
		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};
	
	
	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4{ 1.f };
		Ref<Texture2D> Texture = nullptr;
		float TilingFactor = 1.f;
		
		COMPONENT_DEFAULT_CTR(SpriteRendererComponent);
		
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
			
		SpriteRendererComponent(const Ref<Texture2D>& texture, float tilingFactor = 1.f)
			: Texture(texture), TilingFactor(tilingFactor) {}
	};

	struct CameraComponent
	{
		RenderCamera Camera;
		
		COMPONENT_DEFAULT_CTR(CameraComponent);
		
		CameraComponent(const RenderCamera& camera)
			: Camera{ camera } {}
		
		CameraComponent(const glm::mat4& projection)
			: Camera{ projection } {}
	};
	
}

#undef COMPONENT_DEFAULT_CTR
