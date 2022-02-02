#include "apex_pch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Apex {

	constexpr static auto APEX_GL_STORAGE_BITS = GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

	//////////////////////////////////////////////////////////////////////
	/*----------------------------Buffer--------------------------------*/
	//////////////////////////////////////////////////////////////////////
	OpenGLBuffer::OpenGLBuffer(void* data, size_t size)
		: m_Size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, size, data, APEX_GL_STORAGE_BITS);
	}

	OpenGLBuffer::~OpenGLBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
		
	void OpenGLBuffer::SetData(const void* data, size_t size, size_t offset) const
	{
		APEX_CORE_ASSERT((offset + size <= m_Size), "Size of data exceeding buffer size");
		glNamedBufferSubData(m_RendererID, static_cast<int64_t>(offset), static_cast<int64_t>(size), data);
	}
	
	void* OpenGLBuffer::MapBuffer(bool read, bool write) const
	{
		return glMapNamedBuffer(m_RendererID, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY);
	}
	
	void OpenGLBuffer::UnmapBuffer() const
	{
		glUnmapNamedBuffer(m_RendererID);
	}

	//////////////////////////////////////////////////////////////////////
	/*-------------------------Vertex Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////
	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size)
		: OpenGLBuffer(nullptr, size)
	{
	}
	
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, size_t size)
		: OpenGLBuffer(vertices, size)
	{
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	//////////////////////////////////////////////////////////////////////
	/*-------------------------Index Buffer-----------------------------*/
	//////////////////////////////////////////////////////////////////////
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: OpenGLBuffer(indices, sizeof(uint32_t) * count), m_Count(count)
	{
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	///////////////////////////////////////////////////////////////////////
	/*-------------------------Uniform Buffer----------------------------*/
	///////////////////////////////////////////////////////////////////////
	OpenGLUniformBuffer::OpenGLUniformBuffer(size_t size, uint32_t binding)
		: OpenGLBuffer(nullptr, size)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	void OpenGLUniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}

	void OpenGLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	
	///////////////////////////////////////////////////////////////////////
	/*---------------------Shader Storage Buffer-------------------------*/
	///////////////////////////////////////////////////////////////////////
	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(size_t size, uint32_t binding)
		: OpenGLBuffer(nullptr, size)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_RendererID);
	}

	void OpenGLShaderStorageBuffer::Bind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	}

	void OpenGLShaderStorageBuffer::Unbind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void OpenGLShaderStorageBuffer::ResetData(size_t size)
	{
		glNamedBufferData(m_RendererID, static_cast<int64_t>(size), nullptr, APEX_GL_STORAGE_BITS);
	}

}
