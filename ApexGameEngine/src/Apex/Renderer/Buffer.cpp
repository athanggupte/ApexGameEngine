#include "apex_pch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Apex{

	Ref<VertexBuffer> VertexBuffer::Create(float * vertices, size_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLVertexBuffer>(vertices, size);

			default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t * indices, size_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLIndexBuffer>(indices, count);

			default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
	}

}