#pragma once

#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

namespace Apex {

	// Forward Declarations
	class OrthographicCamera;
	class VertexArray;
	class Texture2D;

// 	struct TransformData
// 	{
// 		glm::vec3 position;
// 		glm::vec2
// 	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		
		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color); // 2D coords, flat color
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color); // 3D coords, flat color
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // 2D coords, textured
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // 3D coords, textured
		
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color); // 2D coords, 2D rotation, flat color
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color); // 3D coords, 2D rotation, flat color
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // 2D coords, 2D rotation, textured
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // 3D coords, 2D rotation, textured
		
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color); // Transform matrix, flat color
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // Transform matrix, textured
		
	private:
// 		struct SceneData
// 		{
// 			glm::mat4 ViewProjectionMatrix;
// 			bool ImageAccess = false;
// 		};
// 
// 		static SceneData* s_SceneData;
	};

}
