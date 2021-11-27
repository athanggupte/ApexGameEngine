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

	[[nodiscard]] constexpr uint32_t ShaderDataTypeSize(const ShaderDataType type)
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
			case ShaderDataType::None:
				APEX_CORE_CRITICAL("Unknown ShaderDataType!");
				return 0;
		}
		return 0;
	}

	[[nodiscard]] constexpr uint32_t ShaderDataTypeComponentCount(const ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Int: return 1;
			case ShaderDataType::Int2: return 2;
			case ShaderDataType::Int3: return 3;
			case ShaderDataType::Int4: return 4;
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Mat2: return 2 * 2;
			case ShaderDataType::Mat3: return 3 * 3;
			case ShaderDataType::Mat4: return 4 * 4;
			case ShaderDataType::Bool: return 1;
			case ShaderDataType::None:
				APEX_CORE_CRITICAL("Unknown ShaderDataType!");
				return 0;
		}
		return 0;
	}

#include "ShaderDefines.h"

	enum class VertexElementType : uint32_t
	{
		Position = ATTRIB_LOC_Position,
		UV0 = ATTRIB_LOC_UV0,
		Color = ATTRIB_LOC_Color,
		TextureIndex = ATTRIB_LOC_TexIndex,
		Normal = ATTRIB_LOC_Normal,
		Tangent = ATTRIB_LOC_Tangent,
		Bitangent = ATTRIB_LOC_Bitangent,
		UV1 = ATTRIB_LOC_UV0,

		UserDefined = ATTRIB_LOC_UserDefined,

		// For counting purposes
		SIZE
	};

	/*-------------------------Buffer Element---------------------------*/
	struct BufferElement
	{
		ShaderDataType e_DataType;
		VertexElementType e_Type;
		uint32_t e_Size;
		uint32_t e_Offset;
		bool e_Normalized;

		//BufferElement() {}

		BufferElement(ShaderDataType data_type, VertexElementType type, bool normalized = false)
			: e_DataType(data_type), e_Type(type), e_Size(ShaderDataTypeSize(data_type)), e_Offset(0), e_Normalized(normalized)
		{
		}
	};


	/*-------------------------Buffer Layout----------------------------*/
	class BufferLayout
	{
	public:
		BufferLayout() = default;

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements) 
		{
			UpdateElementTypes();
			CalculateOffsetAndStride();
		}

		void AddElement(BufferElement element);

		[[nodiscard]] bool HasVertexElementType(VertexElementType type) const;

		[[nodiscard]] const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		[[nodiscard]] uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		[[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		[[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void UpdateElementTypes();
		void CalculateOffsetAndStride();

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
		uint16_t m_ElementTypes = 0;
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
		[[nodiscard]] virtual uint32_t GetCount() const = 0;

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
