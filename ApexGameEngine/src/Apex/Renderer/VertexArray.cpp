#include "apex_pch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Apex {

	VertexArray * VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RenderAPI::OpenGL:	return new OpenGLVertexArray();

			default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

}