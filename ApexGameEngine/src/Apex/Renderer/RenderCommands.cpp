#include "apex_pch.h"
#include "RenderCommands.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Apex {

	RendererAPI* RenderCommands::s_RendererAPI = new OpenGLRendererAPI();

	void RenderCommands::Init()
	{
		s_RendererAPI->Init();
	}

}