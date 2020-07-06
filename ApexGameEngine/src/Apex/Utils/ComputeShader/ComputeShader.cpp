#include <apex_pch.h>
#include "ComputeShader.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLComputeShader.h"

namespace Apex {

	Ref<ComputeShader> ComputeShader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLComputeShader>(filepath);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

	Ref<ComputeShader> ComputeShader::Create(const std::string& name, const std::string& source)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLComputeShader>(name, source);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

}