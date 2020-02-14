#include "apex_pch.h"
#include "FrameBuffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Apex {

	Ref<FrameBuffer> FrameBuffer::Create(bool depth)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLFrameBuffer>(depth);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

	Ref<DepthBuffer> DepthBuffer::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLDepthBuffer>();

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

}