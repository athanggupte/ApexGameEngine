#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define COMPONENT_DEFAULT_CTR(name)	name() = default;\
									name(const name&) = default

#define REGISTER_COMPONENT(name) static constexpr const char* TypeName = name;\
								COMPONENT_DEFAULT_CTR(name)

#include "Apex/Core/ECS/Components/SceneCamera.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"

#include "Apex/Core/ECS/ScriptableEntity.h"

namespace Apex {

	struct TagComponent
	{
		StringHandle Tag;
		
		COMPONENT_DEFAULT_CTR(TagComponent);
		
		TagComponent(StringHandle tag)
			: Tag(tag) {}
		
		operator std::string_view () { return Tag.str(); }
	};
	
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.f, 0.f, 0.f };
		glm::vec3 Rotation = { 0.f, 0.f, 0.f };
		glm::vec3 Scale = { 1.f, 1.f, 1.f };
		
		//TransformComponent() = default;
		//TransformComponent(const TransformComponent&) = default;
		COMPONENT_DEFAULT_CTR(TransformComponent);
		
		TransformComponent(const glm::vec3& translation, const glm::vec3& scale = { 1.f, 1.f, 1.f })
			: Translation(translation), Scale(scale) {}
		
		glm::mat4 GetTransform()
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.f), Rotation.x, { 1.f, 0.f, 0.f })
							   * glm::rotate(glm::mat4(1.f), Rotation.y, { 0.f, 1.f, 0.f })
							   * glm::rotate(glm::mat4(1.f), Rotation.z, { 0.f, 0.f, 1.f });
			
			return glm::translate(glm::mat4(1.f), Translation)
				 * rotation
				 * glm::scale(glm::mat4(1.f), Scale);
		}
	};
	
	
	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4{ 1.f };
		Handle Texture = 0;
		float TilingFactor = 1.f;
		bool useTexture = true;
		bool visible = true;
		
		COMPONENT_DEFAULT_CTR(SpriteRendererComponent);
		
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
		
		SpriteRendererComponent(Handle texture, float tilingFactor = 1.f)
			: Texture(texture), TilingFactor(tilingFactor) {}
		
		/*
		SpriteRendererComponent(const Ref<Apex::Texture>& texture, float tilingFactor = 1.f)
			: Texture(std::dynamic_pointer_cast<Texture2D>(texture)), TilingFactor(tilingFactor) {}

		SpriteRendererComponent(const Ref<Texture2D>& texture, float tilingFactor = 1.f)
			: Texture(texture), TilingFactor(tilingFactor) {}
		*/
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool FixedAspectRatio = false;
		
		COMPONENT_DEFAULT_CTR(CameraComponent);
		
		CameraComponent(const SceneCamera& camera)
			: Camera{ camera } {}
		
// 		CameraComponent(const glm::mat4& projection)
// 			: Camera{ projection } {}
	};
	
	
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance;
		
		std::function<void(Entity, Scene*)> InstantiateFn;
		std::function<void()> DestroyFn;
		
		template<typename Instance_t>
		void Bind()
		{
			
		}
		
	};
	
	struct ScriptComponent
	{
		// Temporary : Not functional. only meant for UI in inspector panel
		std::string Filename;
	};
	
}
