#include "apex_pch.h"
#include "Renderer.h"

namespace Apex {

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}

}