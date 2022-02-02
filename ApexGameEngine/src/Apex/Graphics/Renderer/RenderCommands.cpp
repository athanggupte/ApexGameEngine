#include "apex_pch.h"
#include "RenderCommands.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Apex {

	RendererAPI* RenderCommands::s_RendererAPI = new OpenGLRendererAPI();

	void RenderCommands::Init()
	{
		s_RendererAPI->Init();
		APEX_CORE_TRACE("Apex::RenderCommands initialized successfully!");
	}

	void RenderCommands::SetClearColor(const glm::vec4& color)
	{ s_RendererAPI->SetClearColor(color); }

	void RenderCommands::Clear()
	{ s_RendererAPI->Clear(); }

	void RenderCommands::Draw(uint32_t vertexCount)
	{ s_RendererAPI->Draw(vertexCount); }

	void RenderCommands::Draw(const Ref<VertexArray>& vertexArray)
	{ s_RendererAPI->Draw(vertexArray); }

	void RenderCommands::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{ s_RendererAPI->DrawIndexed(vertexArray, indexCount); }

	void RenderCommands::DrawInstanced(const Ref<VertexArray>& vertexArray, uint32_t count)
	{ s_RendererAPI->DrawInstanced(vertexArray, count); }

	void RenderCommands::SetDepthTest(bool value)
	{ s_RendererAPI->SetDepthTest(value); }

	void RenderCommands::SetDepthWriteMode(bool value)
	{ s_RendererAPI->SetDepthWriteMode(value); }

	void RenderCommands::SetDepthTestFunction(DepthStencilMode criterion)
	{ s_RendererAPI->SetDepthTestFunction(criterion); }

	void RenderCommands::SetStencilTest(bool value)
	{ s_RendererAPI->SetStencilTest(value); }

	void RenderCommands::SetStencilTestFunction(DepthStencilMode criterion, int8_t refValue, uint8_t mask)
	{ s_RendererAPI->SetStencilTestFunction(criterion, refValue, mask); }

	void RenderCommands::SetStencilWriteMask(uint32_t mask)
	{ s_RendererAPI->SetStencilWriteMask(mask); }

	void RenderCommands::SetStencilAction(DepthStencilAction onStencilFail, DepthStencilAction onDepthFail,
		DepthStencilAction onPass)
	{ s_RendererAPI->SetStencilAction(onStencilFail, onDepthFail, onPass); }

	void RenderCommands::SetCulling(bool value)
	{ s_RendererAPI->SetCulling(value); }

	void RenderCommands::SetBlendMode(BlendingMode sourceMode, BlendingMode destMode)
	{ s_RendererAPI->SetBlendMode(sourceMode, destMode); }

	void RenderCommands::SetViewport(uint32_t x, uint32_t y, size_t width, size_t height)
	{ s_RendererAPI->SetViewport(x, y, width, height); }

	void RenderCommands::ImageAccessBarrier()
	{ s_RendererAPI->ImageAccessBarrier(); }
}
