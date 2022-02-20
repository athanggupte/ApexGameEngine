#include "apex_pch.h"
#include "Buffer.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Apex{

	void BufferLayout::AddElement(BufferElement element)
	{
		element.e_Offset = 0;
		if (!m_Elements.empty()) {
			const auto& lastElement = m_Elements.back();
			element.e_Offset = lastElement.e_Offset + lastElement.e_Size;
		}
		m_Stride += element.e_Size;
		m_Elements.push_back(element);
		m_ElementTypes |= static_cast<uint16_t>(element.e_Type);
	}

	bool BufferLayout::HasVertexElementType(VertexElementType type) const
	{
		return (m_ElementTypes & (1 << static_cast<uint16_t>(type)));
	}

	void BufferLayout::UpdateElementTypes()
	{
		for (const auto& element : m_Elements) {
			m_ElementTypes |= (1 << static_cast<uint16_t>(element.e_Type));
		}
	}

	void BufferLayout::CalculateOffsetAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements) {
			element.e_Offset = offset;
			offset += element.e_Size;
			m_Stride += element.e_Size;
			//APEX_CORE_TRACE("Element::{0} | Type::{1} | Offset::{2} | Stride::{3}", element.e_Name, (uint32_t)element.e_DataType, element.e_Offset, m_Stride);
		}
	}

	/*-------------------------Vertex Buffer-----------------------------*/
	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(size);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float * vertices, size_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	/*-------------------------Index Buffer-----------------------------*/
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, size_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLIndexBuffer>(indices, count);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	/*-------------------------Uniform Buffer-----------------------------*/
	Ref<UniformBuffer> UniformBuffer::Create(size_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLUniformBuffer>(size, binding);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

	/*-------------------------Shader Storage Buffer-----------------------------*/
	Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(size_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShaderStorageBuffer>(size, binding);
		}
		APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
	}

}
