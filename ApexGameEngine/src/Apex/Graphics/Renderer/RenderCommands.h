#pragma once

#include "RendererAPI.h"

namespace Apex {

	class RenderCommands
	{
	public:
		static void Init();

		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void Draw(uint32_t vertexCount);

		static void Draw(const Ref<VertexArray>& vertexArray);
		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);
		static void DrawInstanced(const Ref<VertexArray>& vertexArray, uint32_t count);

		static void SetDepthTest(bool value);
		static void SetDepthWriteMode(bool value);
		static void SetDepthTestFunction(DepthStencilMode criterion);

		static void SetStencilTest(bool value);
		static void SetStencilTestFunction(DepthStencilMode criterion, int8_t refValue, uint8_t mask = 0xff);
		static void SetStencilWriteMask(uint32_t mask = 0xff);
		static void SetStencilAction(DepthStencilAction onStencilFail = DepthStencilAction::KEEP,
		                                    DepthStencilAction onDepthFail = DepthStencilAction::KEEP,
		                                    DepthStencilAction onPass = DepthStencilAction::KEEP);

		static void SetCulling(bool value);
		static void SetBlendMode(BlendingMode sourceMode, BlendingMode destMode);
		static void SetViewport(uint32_t x, uint32_t y, size_t width, size_t height);
		static void ImageAccessBarrier();

	private:
		static RendererAPI* s_RendererAPI;
	};

}
