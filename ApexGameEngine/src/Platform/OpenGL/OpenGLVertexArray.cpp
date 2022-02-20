#include "apex_pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Apex {
	
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Int:			
			case ShaderDataType::Int2:		
			case ShaderDataType::Int3:		
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Float:		
			case ShaderDataType::Float2:		
			case ShaderDataType::Float3:		
			case ShaderDataType::Float4:		
			case ShaderDataType::Mat2:		
			case ShaderDataType::Mat3:		
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Bool:		return GL_BOOL;
			default:	APEX_CORE_CRITICAL("Unknown ShaderDataType!");	return 0;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
		// glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::BindIndexBuffer(uint32_t indexBufferIndex) const
	{
		APEX_CORE_ASSERT(m_IndexBuffers.size() > indexBufferIndex, "Invalid IndexBuffer index!");
		glVertexArrayElementBuffer(m_RendererID, m_IndexBuffers[indexBufferIndex]->GetHandle());
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer)
	{
		// glBindVertexArray(m_RendererID);
		// vertexBuffer->Bind();
		glVertexArrayVertexBuffer(m_RendererID, m_VertexBuffers.size(), vertexBuffer->GetHandle(), 0, vertexBuffer->GetLayout().GetStride());

		APEX_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex Buffer has no layout!");

		const BufferLayout& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout) {
			const uint32_t attribLocation = (element.e_Type == VertexElementType::UserDefined) ? static_cast<uint32_t>(element.e_Type) + m_NumAttribs++ : static_cast<uint32_t>(element.e_Type);
			glEnableVertexArrayAttrib(m_RendererID, attribLocation);
			glVertexArrayAttribFormat(m_RendererID, attribLocation,
				ShaderDataTypeComponentCount(element.e_DataType),
				ShaderDataTypeToOpenGLBaseType(element.e_DataType),
				element.e_Normalized ? GL_TRUE : GL_FALSE,
				element.e_Offset);
			glVertexArrayAttribBinding(m_RendererID, attribLocation, m_VertexBuffers.size());
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::AddIndexBuffer(const Ref<IndexBuffer> indexBuffer)
	{
		glVertexArrayElementBuffer(m_RendererID, indexBuffer->GetHandle());

		m_IndexBuffers.push_back(indexBuffer);
	}

}