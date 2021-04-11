#include "apex_pch.h"
#include "Renderer.h"

#include "RenderCommands.h"
#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/Model/Model.h"

namespace Apex {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommands::Init();
		APEX_CORE_TRACE("Apex::Renderer initialized successfully!");
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		if (s_SceneData->ImageAccess)
			RenderCommands::ImageAccessBarrier();
	}
	
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix)
	{
		shader->Bind();
		shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUniMat4("u_Model", modelMatrix);

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}

	void Renderer::SubmitArray(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, glm::mat4 modelMatrices[], size_t count)
	{
		shader->Bind();
		shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUniMat4v("u_Model", modelMatrices, count);

		vertexArray->Bind();
		RenderCommands::DrawInstanced(vertexArray, count);
	}

	/*void Renderer::SubmitModel(const Ref<Shader>& shader, const Ref<Model>& model, const glm::mat4 & modelMatrix)
	{
		model->ApplyModelMatrix(modelMatrix);
		shader->Bind();
		shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUniMat4("u_Model", model->GetModelMatrix());

		for (auto&[name, mesh] : model->GetMeshes()) {
			if (mesh->Show()) {
				uint32_t i = 1;
				//APEX_CORE_INFO("Mesh textures ->");
				for (auto[name, texture] : mesh->GetTextures()) {
					//APEX_CORE_INFO("{0} : {1} : {2}", name, texture->GetPath(), i);
					shader->SetUniInt1("u_" + name, i);
					texture->Bind(i);
					i++;
				}
				mesh->GetVAO()->Bind();
				RenderCommands::DrawIndexed(mesh->GetVAO());
			}
		}
	}*/

#if 0
	/*--------------------------------------------------------------------------------------------------*/
	void Renderer::SubmitPostProcess(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray)
	{
		shader->Bind();
		vertexArray->Bind();
		RenderCommands::Draw(vertexArray);
	}
	/*--------------------------------------------------------------------------------------------------*/
#endif

	void Renderer::Shutdown()
	{
		delete s_SceneData;
	}
	
}
