#include "apex_pch.h"
#include "Buffer.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Apex{

	/*-------------------------Vertex Buffer-----------------------------*/
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(size);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float * vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	/*-------------------------Index Buffer-----------------------------*/
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t * indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLIndexBuffer>(indices, count);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	/*-------------------------Uniform Buffer-----------------------------*/
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLUniformBuffer>(size, binding);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

}
