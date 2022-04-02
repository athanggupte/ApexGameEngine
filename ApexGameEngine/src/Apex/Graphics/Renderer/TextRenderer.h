#pragma once

#include "Apex/Graphics/Font.h"

#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>


struct ImFontAtlas;

namespace Apex {

	// Forward Declarations
	class Camera;
	class VertexArray;
	class Texture2D;

// 	struct TransformData
// 	{
// 		glm::vec3 position;
// 		glm::vec2
// 	};

	class TextRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();
		
		// Primitives
		static void DrawGlyphs(const ::std::string& text, const glm::mat4& transform, const ::Apex::Font& font, const glm::vec4& color, float width = 0.52f, float thickness = 0.f);
		//static void DrawGlyphs(const std::string& text, const glm::mat4& transform, const Font& font, const TextGradient& gradient, float thickness = 0.f);
		
		struct Stats
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;
			
			uint32_t GetVertexCount() { return quadCount * 4; }
			uint32_t GetIndexCount() { return quadCount * 6; }
			uint32_t GetTriangleCount() { return quadCount * 2; }
		};
		static void ResetStats();
		static Stats GetStats();
	
	private:
		static void ResetBatch();
		static void FlushBatch();
		
	};

}

