#pragma once

#include "RendererAPI.h"

#include <glm/glm.hpp>

namespace Apex {
	
	// Forward Declarations
	class Camera;
	class VertexArray;
	class Shader;
	
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix = glm::mat4(1.0f));
		static void SubmitArray(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, glm::mat4 modelMatrices[], size_t count);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }


		/* Temporary */
		static void SubmitPostProcess(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);
		static void SetImageAccessBit() { s_SceneData->ImageAccess = true; }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 InverseViewMatrix;
			bool ImageAccess = false;
		};

		static SceneData* s_SceneData;
	};
	
}
