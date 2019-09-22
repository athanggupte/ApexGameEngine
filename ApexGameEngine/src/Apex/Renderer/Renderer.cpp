#include "apex_pch.h"
#include "Renderer.h"

namespace Apex {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommands::Init();
		APEX_CORE_TRACE("Apex::Renderer initialized successfully!");
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix)
	{
		shader->Bind();
		shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUniMat4("u_Model", modelMatrix);

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}

	/*--------------------------------------------------------------------------------------------------*/
	void Renderer::SubmitPostProcess(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray)
	{
		shader->Bind();
		vertexArray->Bind();
		RenderCommands::Draw(vertexArray);
	}
	/*--------------------------------------------------------------------------------------------------*/

}