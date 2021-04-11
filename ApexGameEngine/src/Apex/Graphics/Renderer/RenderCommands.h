#pragma once

#include "RendererAPI.h"

namespace Apex {

	class RenderCommands
	{
	public:
		static void Init();

		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void Draw(const Ref<VertexArray>& vertexArray) { s_RendererAPI->Draw(vertexArray); }
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) { s_RendererAPI->DrawIndexed(vertexArray, indexCount); }
		inline static void DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count) { s_RendererAPI->DrawInstanced(vertexArray, count); }
		
		inline static void SetDepthTest(bool value) { s_RendererAPI->SetDepthTest(value); }
		inline static void SetDepthWriteMode(bool value) { s_RendererAPI->SetDepthWriteMode(value); }
		inline static void SetDepthTestFunction(DepthStencilMode criterion) { s_RendererAPI->SetDepthTestFunction(criterion); }
		
		inline static void SetStencilTest(bool value) { s_RendererAPI->SetStencilTest(value); }
		inline static void SetStencilTestFunction(DepthStencilMode criterion, int8_t refValue, uint8_t mask = 0xff) { s_RendererAPI->SetStencilTestFunction(criterion, refValue, mask); }
		inline static void SetStencilWriteMask(uint32_t mask = 0xff) { s_RendererAPI->SetStencilWriteMask(mask); }
		inline static void SetStencilAction(DepthStencilAction onStencilFail = DepthStencilAction::KEEP, DepthStencilAction onDepthFail = DepthStencilAction::KEEP, DepthStencilAction onPass = DepthStencilAction::KEEP) { s_RendererAPI->SetStencilAction(onStencilFail, onDepthFail, onPass); }
		
		inline static void SetCulling(bool value) { s_RendererAPI->SetCulling(value); }
		inline static void SetBlendMode(BlendingMode sourceMode, BlendingMode destMode) { s_RendererAPI->SetBlendMode(sourceMode, destMode); }
		inline static void SetViewport(uint32_t x, uint32_t y, size_t width, size_t height) { s_RendererAPI->SetViewport(x, y, width, height); }
		inline static void ImageAccessBarrier() { s_RendererAPI->ImageAccessBarrier(); }

	private:
		static RendererAPI* s_RendererAPI;
	};

}
