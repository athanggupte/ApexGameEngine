#pragma once

#include "RendererAPI.h"

namespace Apex {

	class APEX_API RenderCommands
	{
	public:
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }
		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }

	private:
		static RendererAPI* s_RendererAPI;
	};

}