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
		StringHandle tag;
		
		COMPONENT_DEFAULT_CTR(TagComponent);
		
		TagComponent(StringHandle tag)
			: tag(tag) {}
		
		operator std::string_view () { return tag.str(); }
	};
	
	struct TransformComponent
	{
		glm::vec3 translation = { 0.f, 0.f, 0.f };
		glm::vec3 rotation = { 0.f, 0.f, 0.f };
		glm::vec3 scale = { 1.f, 1.f, 1.f };
		
		//TransformComponent() = default;
		//TransformComponent(const TransformComponent&) = default;
		COMPONENT_DEFAULT_CTR(TransformComponent);
		
		TransformComponent(const glm::vec3& _translation, const glm::vec3& _scale = { 1.f, 1.f, 1.f })
			: translation(_translation), scale(_scale) {}
		
		glm::mat4 GetTransform()
		{
			glm::mat4 _rotation = glm::rotate(glm::mat4(1.f), rotation.x, { 1.f, 0.f, 0.f })
							   * glm::rotate(glm::mat4(1.f), rotation.y, { 0.f, 1.f, 0.f })
							   * glm::rotate(glm::mat4(1.f), rotation.z, { 0.f, 0.f, 1.f });
			
			return glm::translate(glm::mat4(1.f), translation)
				 * _rotation
				 * glm::scale(glm::mat4(1.f), scale);
		}
	};
	
	
	struct SpriteRendererComponent
	{
		glm::vec4 color = glm::vec4{ 1.f };
		Handle texture = 0;
		float tilingFactor = 1.f;
		bool useTexture = true;
		bool visible = true;
		
		COMPONENT_DEFAULT_CTR(SpriteRendererComponent);
		
		SpriteRendererComponent(const glm::vec4& _color)
			: color(_color) {}
		
		SpriteRendererComponent(Handle _texture, float _tilingFactor = 1.f)
			: texture(_texture), tilingFactor(_tilingFactor) {}
		
		/*
		SpriteRendererComponent(const Ref<Apex::Texture>& texture, float tilingFactor = 1.f)
			: Texture(std::dynamic_pointer_cast<Texture2D>(texture)), TilingFactor(tilingFactor) {}

		SpriteRendererComponent(const Ref<Texture2D>& texture, float tilingFactor = 1.f)
			: Texture(texture), TilingFactor(tilingFactor) {}
		*/
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool fixedAspectRatio = false;
		
		COMPONENT_DEFAULT_CTR(CameraComponent);

		explicit CameraComponent(const SceneCamera& camera)
			: camera{ camera } {}
		
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
		std::string filename;
	};
	
}
