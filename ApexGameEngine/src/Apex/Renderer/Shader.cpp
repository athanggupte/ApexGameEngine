#include "apex_pch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Apex {
	
	Ref<Shader> Shader::Create(const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}
}