#include "apex_pch.h"
#include "Renderer.h"

namespace Apex {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->Bind();
		shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}

}