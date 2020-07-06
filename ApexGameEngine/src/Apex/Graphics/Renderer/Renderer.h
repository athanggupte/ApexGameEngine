#pragma once

#include "RenderCommands.h"

#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/Model/Model.h"

#include <glm/glm.hpp>

namespace Apex {

	class Renderer
	{
	public:
		static void Init();
		static void BeginScene(Camera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix = glm::mat4(1.0f));
		static void SubmitArray(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, glm::mat4 modelMatrices[], size_t count);
		static void SubmitModel(const Ref<Shader>& shader, const Ref<Model>& model, const glm::mat4& modelMatrix = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }


		/* Temporary */
		static void SubmitPostProcess(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);
		static void SetImageAccessBit() { s_SceneData->ImageAccess = true; }


	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			bool ImageAccess = false;
		};

		static SceneData* s_SceneData;
	};
}