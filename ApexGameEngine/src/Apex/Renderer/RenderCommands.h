#pragma once

#include "RendererAPI.h"

namespace Apex {

	class RenderCommands
	{
	public:
		static void Init();

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }
		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

	private:
		static RendererAPI* s_RendererAPI;
	};

}