#pragma once

namespace Apex {

	enum class RenderAPI
	{
		None = 0, OpenGL = 1
	};

	class APEX_API Renderer
	{
	public:
		inline static RenderAPI GetAPI() { return s_RenderAPI; }
		inline static RenderAPI SetAPI(RenderAPI renderAPI) { s_RenderAPI = renderAPI; }
	private:
		static RenderAPI s_RenderAPI;
	};
}