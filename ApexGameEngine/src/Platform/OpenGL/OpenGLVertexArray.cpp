#include "apex_pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Apex {
	
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Apex::ShaderDataType::Int:			return GL_INT;
			case Apex::ShaderDataType::Int2:		return GL_INT;
			case Apex::ShaderDataType::Int3:		return GL_INT;
			case Apex::ShaderDataType::Int4:		return GL_INT;
			case Apex::ShaderDataType::Float:		return GL_FLOAT;
			case Apex::ShaderDataType::Float2:		return GL_FLOAT;
			case Apex::ShaderDataType::Float3:		return GL_FLOAT;
			case Apex::ShaderDataType::Float4:		return GL_FLOAT;
			case Apex::ShaderDataType::Mat2:		return GL_FLOAT;
			case Apex::ShaderDataType::Mat3:		return GL_FLOAT;
			case Apex::ShaderDataType::Mat4:		return GL_FLOAT;
			case Apex::ShaderDataType::Bool:		return GL_BOOL;
			default:	APEX_CORE_CRITICAL("Unknown ShaderDataType!");	return 0;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		APEX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		uint32_t index = 0;
		const BufferLayout& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.e_Type),
				element.e_Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.e_Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddIndexBuffer(const Ref<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffers.push_back(indexBuffer);
		glBindVertexArray(0);
	}

}