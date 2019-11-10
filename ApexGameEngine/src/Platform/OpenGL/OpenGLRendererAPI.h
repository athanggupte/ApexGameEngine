#pragma once

#include "Apex/Renderer/RendererAPI.h"

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

	};

}