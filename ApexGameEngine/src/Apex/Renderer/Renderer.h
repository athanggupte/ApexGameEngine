#pragma once

#include "RenderCommands.h"

#include "Camera.h"
#include "Shader.h"
#include "Apex/Model/Model.h"

#include <glm/glm.hpp>

namespace Apex {

	class Renderer
	{
	public:
		static void Init();
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix = glm::mat4(1.0f));
		static void SubmitArray(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, glm::mat4 modelMatrices[], size_t count);
		static void SubmitModel(const Ref<Shader>& shader, const Ref<Model>& model, const glm::mat4& modelMatrix = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }


		/* Temporary */	static void SubmitPostProcess(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);


	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};
}