#include "apex_pch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Apex {
	
	Shader * Shader::Create(const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RenderAPI::OpenGL:	return new OpenGLShader(vertexSrc, fragmentSrc);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}
}