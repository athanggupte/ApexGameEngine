#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define COMPONENT_DEFAULT_CTR(name)	name() = default;\
									name(const name&) = default

#define REGISTER_COMPONENT(name) static constexpr const char* TypeName = name;\
								COMPONENT_DEFAULT_CTR(name)

#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"

#include "Apex/Core/ECS/EntityScript.h"

namespace Apex {

	struct IDComponent
	{
		GUID id;
	};

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

		[[nodiscard]] glm::mat4 GetTransform() const
		{
			const glm::mat4 _rotation = glm::rotate(glm::mat4(1.f), rotation.x, { 1.f, 0.f, 0.f })
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
		Resource<Texture> texture;
		float tilingFactor = 1.f;
		bool useTexture = true;
		bool visible = true;
		
		COMPONENT_DEFAULT_CTR(SpriteRendererComponent);
		
		SpriteRendererComponent(const glm::vec4& _color)
			: color(_color) {}
		
		SpriteRendererComponent(const Resource<Texture>& _texture, float _tilingFactor = 1.f)
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
		Camera camera{ Camera::ProjectionType::Orthographic };
		bool fixedAspectRatio = false;
		
		COMPONENT_DEFAULT_CTR(CameraComponent);

		explicit CameraComponent(const Camera& camera)
			: camera{ camera } {}
		
// 		CameraComponent(const glm::mat4& projection)
// 			: Camera{ projection } {}
	};
	
	
	struct NativeScriptComponent
	{
		EntityScript* instance = nullptr;
		Resource<AScriptFactory> factory;

		COMPONENT_DEFAULT_CTR(NativeScriptComponent);

		explicit NativeScriptComponent(const Resource<AScriptFactory>& script_factory)
			: factory(script_factory) {}

		/*using InstantiateFn_t = EntityScript*(*)();
		using DestroyFn_t = void (*)(NativeScriptComponent*);

		InstantiateFn_t InstantiateScript;
		DestroyFn_t DestroyScript;

		void Bind()
		{
			 InstantiateScript = []() { return static_cast<EntityScript*>(new Script_t()); };
			 DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
		}*/
		
	};
	
}
