#pragma once

#include "Apex/Graphics/RenderPrimitives/Buffer.h"

namespace Apex {

	/*-------------------------Buffer----------------------------*/
	class OpenGLBuffer : virtual public Buffer
	{
	public:
		OpenGLBuffer(void* data, uint32_t size);
		virtual ~OpenGLBuffer() override;

		[[nodiscard]] uint32_t GetHandle() const override { return m_RendererID; }
		[[nodiscard]] virtual uint32_t GetSize() const override { return m_Size; }

		void SetData(const void* data, uint32_t size, uint32_t offset) const override;
		[[nodiscard]] void* MapBuffer(bool read, bool write) const override;
		void UnmapBuffer() const override;

	protected:
		uint32_t m_RendererID = 0;
		uint32_t m_Size;
	};

	/*-------------------------Vertex Buffer----------------------------*/
	class OpenGLVertexBuffer final : public VertexBuffer, OpenGLBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);

		void Bind() const override;
		void Unbind() const override;

		[[nodiscard]] const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; m_Count = (uint32_t)((float)m_Size / (float)m_Layout.GetStride()); }

		[[nodiscard]] uint32_t GetCount() const override { return m_Count; }

		// Explicitly specify which base class functions to use
		// To suppress warning: C4250 inherits by dominance
		[[nodiscard]] uint32_t GetHandle() const override { return OpenGLBuffer::GetHandle(); }
		[[nodiscard]] uint32_t GetSize() const override { return OpenGLBuffer::GetSize(); }
		void SetData(const void* data, uint32_t size, uint32_t offset) const override { return OpenGLBuffer::SetData(data, size, offset); }
		[[nodiscard]] void* MapBuffer(bool read, bool write) const override { return OpenGLBuffer::MapBuffer(read, write); }
		void UnmapBuffer() const override { return OpenGLBuffer::UnmapBuffer(); }
		
	private:
		BufferLayout m_Layout;
		uint32_t m_Count = 0;
	};

	/*-------------------------Index Buffer----------------------------*/
	class OpenGLIndexBuffer final : public IndexBuffer, OpenGLBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

		void Bind() const override;
		void Unbind() const override;

		[[nodiscard]] uint32_t GetCount() const override { return m_Count; }

		// Explicitly specify which base class functions to use
		// To suppress warning: C4250 inherits by dominance
		[[nodiscard]] uint32_t GetHandle() const override { return OpenGLBuffer::GetHandle(); }
		[[nodiscard]] uint32_t GetSize() const override { return OpenGLBuffer::GetSize(); }
		void SetData(const void* data, uint32_t size, uint32_t offset) const override { return OpenGLBuffer::SetData(data, size, offset); }
		[[nodiscard]] void* MapBuffer(bool read, bool write) const override { return OpenGLBuffer::MapBuffer(read, write); }
		void UnmapBuffer() const override { return OpenGLBuffer::UnmapBuffer(); }

	private:
		uint32_t m_Count;
	};

	/*-------------------------Uniform Buffer----------------------------*/
	class OpenGLUniformBuffer final : public UniformBuffer, OpenGLBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		
		void Bind() const override;
		void Unbind() const override;

		// Explicitly specify which base class functions to use
		// To suppress warning: C4250 inherits by dominance
		[[nodiscard]] uint32_t GetHandle() const override { return OpenGLBuffer::GetHandle(); }
		[[nodiscard]] uint32_t GetSize() const override { return OpenGLBuffer::GetSize(); }
		void SetData(const void* data, uint32_t size, uint32_t offset) const override { return OpenGLBuffer::SetData(data, size, offset); }
		[[nodiscard]] void* MapBuffer(bool read, bool write) const override { return OpenGLBuffer::MapBuffer(read, write); }
		void UnmapBuffer() const override { return OpenGLBuffer::UnmapBuffer(); }

	private:
	};

	/*-------------------------Shader Storage Buffer----------------------------*/
	class OpenGLShaderStorageBuffer final : public ShaderStorageBuffer, public OpenGLBuffer
	{
	public:
		OpenGLShaderStorageBuffer(uint32_t size, uint32_t binding);

		void Bind() const override;
		void Unbind() const override;
		
		void ResetData(uint32_t size) override;

		// Explicitly specify which base class functions to use
		// To suppress warning: C4250 inherits by dominance
		[[nodiscard]] uint32_t GetHandle() const override { return OpenGLBuffer::GetHandle(); }
		[[nodiscard]] uint32_t GetSize() const override { return OpenGLBuffer::GetSize(); }
		void SetData(const void* data, uint32_t size, uint32_t offset) const override { return OpenGLBuffer::SetData(data, size, offset); }
		[[nodiscard]] void* MapBuffer(bool read, bool write) const override { return OpenGLBuffer::MapBuffer(read, write); }
		void UnmapBuffer() const override { return OpenGLBuffer::UnmapBuffer(); }

	private:
	};

}
