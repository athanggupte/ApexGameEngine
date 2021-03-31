#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	// Forward Declarations
	class OrthographicCamera;
	class VertexArray;
	class Texture2D;
	
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		
		// Primitives
		static void DrawSimpleQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color); // 2D coords, flat color
		static void DrawSimpleQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color); // 3D coords, flat color
		static void DrawSimpleQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // 2D coords, textured
		static void DrawSimpleQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = { 1.0f, 1.0f, 1.0f, 1.0f }); // 3D coords, textured
		
		
		static void DrawRotatedQuad();
		
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			bool ImageAccess = false;
		};

		static SceneData* s_SceneData;
	}
	
}
