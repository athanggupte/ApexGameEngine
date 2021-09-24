#pragma once

#include "Apex/Graphics/RenderPrimitives/Buffer.h"

namespace Apex {

	/*-------------------------Vertex Buffer----------------------------*/
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;
		virtual void* MapBuffer(bool read, bool write = true) override;
		virtual void UnmapBuffer() override;
		
		inline virtual uint32_t GetCount() const override { return m_Count; }

		inline virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		inline virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
		BufferLayout m_Layout;
	};

	/*-------------------------Index Buffer----------------------------*/
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		inline virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	/*-------------------------Uniform Buffer----------------------------*/
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		virtual void* MapBuffer(bool read, bool write = true) override;
		virtual void UnmapBuffer() override;
		
		virtual uint32_t GetSize() const { return m_Size; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};

	/*-------------------------Shader Storage Buffer----------------------------*/
	class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLShaderStorageBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		virtual void ResetData(uint32_t size) override;
		virtual void* MapBuffer(bool read, bool write = true) override;
		virtual void UnmapBuffer() override;
		
		virtual uint32_t GetSize() const { return m_Size; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};

}
