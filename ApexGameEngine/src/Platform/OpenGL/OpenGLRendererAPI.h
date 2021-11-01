#pragma once

#include "Apex/Graphics/Renderer/RendererAPI.h"

namespace Apex {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void Init() override;

		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;

		void Draw(uint32_t vertexCount) override;
		void Draw(const Ref<VertexArray>& vertexArray) override;
		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
		void DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count) override;

		void SetDepthTest(bool value) override;
		void SetDepthWriteMode(bool value) override;
		void SetDepthTestFunction(DepthStencilMode criterion) override;

		void SetStencilTest(bool value) override;
		void SetStencilTestFunction(DepthStencilMode criterion, int8_t refValue, uint8_t mask) override;
		void SetStencilWriteMask(uint32_t mask) override;
		void SetStencilAction(DepthStencilAction onStencilFail = DepthStencilAction::KEEP, DepthStencilAction onDepthFail = DepthStencilAction::KEEP, DepthStencilAction onPass = DepthStencilAction::KEEP) override;

		void SetCulling(bool value) override;
		void SetBlendMode(BlendingMode sourceMode, BlendingMode destMode) override;
		void SetViewport(uint32_t x, uint32_t y, size_t width, size_t height) override;
		void ImageAccessBarrier() override;
	};

}
