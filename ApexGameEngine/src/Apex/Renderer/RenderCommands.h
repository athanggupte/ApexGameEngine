#pragma once

#include "RendererAPI.h"

namespace Apex {

	enum class RenderCommandType {
		Clear = 0,
		Draw, DrawIndexed, DrawInstanced,
		SetClearColor, SetDepthTest, SetCulling,
		StartScene, EndScene
	};

	class RenderCommands
	{
	public:
		static void Init();

		inline static void Draw(const Ref<VertexArray>& vertexArray) { s_RendererAPI->Draw(vertexArray); }
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }
		inline static void DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count) { s_RendererAPI->DrawInstanced(vertexArray, count); }
		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
		inline static void Clear() { s_RendererAPI->Clear(); }
		inline static void SetDepthTest(bool value) { s_RendererAPI->SetDepthTest(value); }
		inline static void SetCulling(bool value) { s_RendererAPI->SetCulling(value); }

	private:
		static RendererAPI* s_RendererAPI;
	};

}