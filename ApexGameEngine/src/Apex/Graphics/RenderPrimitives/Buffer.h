#pragma once

namespace Apex {

	enum class ShaderDataType
	{
		None = 0,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat2, Mat3, Mat4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat2:		return 4 * 2 * 2;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Bool:		return 1;
			default:	APEX_CORE_CRITICAL("Unknown ShaderDataType!");	return 0;
		}
	}


	/*-------------------------Buffer Element---------------------------*/
	struct BufferElement
	{
		std::string e_Name;
		ShaderDataType e_Type;
		uint32_t e_Size;
		uint32_t e_Offset;
		bool e_Normalized;

		//BufferElement() {}

		BufferElement(ShaderDataType type, const std::string name, bool normalized = false)
			: e_Name(name), e_Type(type), e_Size(ShaderDataTypeSize(type)), e_Offset(0), e_Normalized(normalized)
		{
		}

		[[nodiscard]] uint32_t GetComponentCount() const
		{
			switch (e_Type)
			{
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat2:		return 2 * 2;
				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;
				case ShaderDataType::Bool:		return 1;
				default:	APEX_CORE_CRITICAL("Unknown ShaderDataType!");	return 0;
			}
		}
	};


	/*-------------------------Buffer Layout----------------------------*/
	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements) 
		{
			CalculateOffsetAndStride();
		}

		[[nodiscard]] const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		[[nodiscard]] uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		[[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		[[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.e_Offset = offset;
				offset += element.e_Size;
				m_Stride += element.e_Size;
				//APEX_CORE_TRACE("Element::{0} | Type::{1} | Offset::{2} | Stride::{3}", element.e_Name, (uint32_t)element.e_Type, element.e_Offset, m_Stride);
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	/*-------------------------Buffer----------------------------*/
	class Buffer
	{
	public:
		virtual ~Buffer() = default;

		[[nodiscard]] virtual uint32_t GetHandle() const = 0;
		[[nodiscard]] virtual uint32_t GetSize() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0u) const = 0;
		[[nodiscard]] virtual void* MapBuffer(bool read, bool write = true) const = 0;
		virtual void UnmapBuffer() const = 0;
	};


	/*-------------------------Vertex Buffer----------------------------*/
	class VertexBuffer : virtual public Buffer
	{
	public:
		[[nodiscard]] virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		// Creation utility
		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};


	/*-------------------------Index Buffer-----------------------------*/
	class IndexBuffer : virtual public Buffer
	{
	public:
		[[nodiscard]] virtual uint32_t GetCount() const = 0;

		// Creation utility
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};


	/*-------------------------Uniform Buffer-----------------------------*/
	class UniformBuffer : virtual public Buffer
	{
	public:
		// Creation utility
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};


	/*-------------------------Shader Storage Buffer-----------------------------*/
	class ShaderStorageBuffer : virtual public Buffer
	{
	public:
		virtual void ResetData(uint32_t size) = 0;
		
		// Creation utility
		static Ref<ShaderStorageBuffer> Create(uint32_t size, uint32_t binding);
	};

}
