#pragma once

#include "Apex/Graphics/Renderer/RendererAPI.h"

namespace Apex {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void Draw(const Ref<VertexArray>& vertexArray) override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
		virtual void DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count) override;

		virtual void SetDepthTest(bool value) override;
		virtual void SetCulling(bool value) override;
		virtual void SetBlendMode(BlendingMode sourceMode, BlendingMode destMode) override;
		virtual void SetViewport(uint32_t x, uint32_t y, size_t width, size_t height) override;
		virtual void ImageAccessBarrier() override;

	};

}