#include "apex_pch.h"
#include "Framebuffer.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Apex {

	Ref<Framebuffer> Framebuffer::Create(FramebufferSpec spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLFramebuffer>(spec);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

#if USE_DEPTH_BUFFER
	Ref<DepthBuffer> DepthBuffer::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLDepthBuffer>();
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}
#endif

}
