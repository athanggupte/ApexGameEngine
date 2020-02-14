#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Apex {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void Draw(const Ref<VertexArray>& vertexArray) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
		virtual void DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count) = 0;

		virtual void SetDepthTest(bool value) = 0;
		virtual void SetCulling(bool value) = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, size_t width, size_t height) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}