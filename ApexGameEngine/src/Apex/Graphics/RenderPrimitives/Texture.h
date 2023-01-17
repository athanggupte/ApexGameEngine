#pragma once
#include "Texture.h"
#include "glm/vec4.hpp"

namespace Apex {
	struct TextureDesc;

	enum class TextureAccessFormat : uint8_t
	{
		RED, 		//I_RED,
		RG, 		//I_RG,
		RGB, BGR, 	//I_RGB, I_BGR,
		RGBA, BGRA, //I_RGBA, I_BGRA

		DEPTH,
		STENCIL,
	};
	constexpr size_t SIZE_TEXTUREACCESSFORMAT = sizeof(TextureAccessFormat);
	static_assert(SIZE_TEXTUREACCESSFORMAT == 1);
	
	enum class TextureInternalFormat : uint8_t
	{
		R8 = 0x10, R16, R32,
		RG8 = 0x20, RG16, RG32,
		RGB8 = 0x30, RGB16, RGB32,
		RGBA8 = 0x40, RGBA16, RGBA32,
		SRGB8 = 0x50, 
		SRGBA8 = 0x60,

		DEPTH16 = 0x70, DEPTH24, DEPTH32,
		D24S8 = 0x90, D32S8,
		STENCIL8 = 0xa0
	};
	constexpr size_t SIZE_TEXTUREINTERNALFORMAT = sizeof(TextureInternalFormat);
	static_assert(SIZE_TEXTUREINTERNALFORMAT == 1);
	
	enum class TextureDataType : uint8_t
	{
		BYTE, UBYTE,
		SHORT, USHORT,
		INT, UINT,
		FLOAT
	};
	constexpr size_t SIZE_TEXTUREDATATYPE = sizeof(TextureDataType);
	static_assert(SIZE_TEXTUREDATATYPE == 1);

	enum class TextureFiltering : uint8_t
	{
		NEAREST,
		LINEAR,
		BILINEAR,
		TRILINEAR,

		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		NEAREST_MIPMAP_NEAREST,
	};
	constexpr size_t SIZE_TEXTUREFILTERING = sizeof(TextureFiltering);
	static_assert(SIZE_TEXTUREFILTERING == 1);

	enum class TextureWrapMode : uint8_t
	{
		REPEAT,
		MIRRORED,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
	};
	constexpr size_t SIZE_TEXTUREWRAPMODE = sizeof(TextureWrapMode);
	static_assert(SIZE_TEXTUREWRAPMODE == 1);

	struct TextureSpec
	{
		TextureAccessFormat accessFormat;
		TextureInternalFormat internalFormat;
		TextureDataType dataType;
		TextureFiltering filtering = TextureFiltering::BILINEAR;
	};
	constexpr size_t SIZE_TEXTURESPEC = sizeof(TextureSpec);
	static_assert(SIZE_TEXTURESPEC == 4);


	namespace defaults {
		// Default TextureSpecs
		inline static constexpr TextureSpec SimpleTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE };
		inline static constexpr TextureSpec SRGBTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::SRGBA8, TextureDataType::UBYTE };
		inline static constexpr TextureSpec HDRTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT };
		inline static constexpr TextureSpec EnvironmentMapSpec{ TextureAccessFormat::RGB, TextureInternalFormat::RGB16, TextureDataType::FLOAT };
	}

	enum class TextureType : uint8_t
	{
		Texture2D,
		TextureCubemap,
		//TextureDepth2D,
		Texture2DMS,

		Texture3D,
		Texture2DArray,
		TextureCubemapArray,
	};
	constexpr size_t SIZE_TEXTURETYPE = sizeof(TextureType);
	static_assert(SIZE_TEXTURETYPE == 1);

	class Texture
	{
	public:
		virtual ~Texture() = default;
		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		[[nodiscard]] virtual uint32_t GetID() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void SetData(void* data, uint32_t size, int level = 0) = 0;
		// virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Invalidate(uint32_t width, uint32_t height) = 0;

		virtual void GenerateMipmap() const = 0;

		[[nodiscard]] virtual std::string_view GetName() const = 0;
		
		//virtual void SetRows(uint32_t rows) { this->m_NumberOfRows = rows; this->m_MaxIndex = rows * rows; }
		//virtual uint32_t GetRows() { return this->m_NumberOfRows; }
		//virtual uint32_t GetMaxIndex() { return this->m_MaxIndex; }
		virtual uint32_t GetMipLevels() { return this->m_MipLevels; }
		//[[nodiscard]] virtual TextureSpec GetSpec() const = 0;
		[[nodiscard]] virtual TextureType GetType() const = 0;

		static Ref<Texture> APEX_API Create(TextureDesc desc);

	protected:
		//uint32_t m_NumberOfRows = 1;// , n_NumberOfCols = 1;
		//uint32_t m_MaxIndex = 1;
		uint32_t m_MipLevels = 1;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> APEX_API Create(const fs::path& path, bool useSRGB = false, bool useHDR = false, TextureFiltering filtering = TextureFiltering::BILINEAR);
		static Ref<Texture2D> APEX_API Create(uint32_t width, uint32_t height, const TextureSpec& spec, const std::string& name = "");

		virtual void BindImage(uint32_t unit, bool read, bool write, int level = 0) const = 0;

		[[nodiscard]] TextureType GetType() const override { return TextureType::Texture2D; }
	};

	class Texture2DMS : public Texture2D
	{
	public:
		static Ref<Texture2DMS> APEX_API Create(uint32_t width, uint32_t height, const TextureSpec& spec, uint32_t samples, const std::string& name = "");

		[[nodiscard]] TextureType GetType() const override { return TextureType::Texture2DMS; }

	private:
		void BindImage(uint32_t unit, bool read, bool write, int level) const override {}
	};

	class TextureCubemap : public Texture
	{
	public:
		static Ref<TextureCubemap> APEX_API Create(const std::array<fs::path, 6>& paths, bool useHDR = true, TextureFiltering filtering = TextureFiltering::BILINEAR);
		static Ref<TextureCubemap> APEX_API Create(uint32_t size, const TextureSpec& spec, const std::string& name = "");

		virtual void SetData(void* data, int faceIndex, uint32_t size, int level = 0) = 0;

		[[nodiscard]] TextureType GetType() const override { return TextureType::TextureCubemap; }
	};
	
	class Texture2DArray : public Texture
	{
	public:
		static Ref<Texture2DArray> APEX_API Create(uint32_t width, uint32_t height, uint32_t count, const TextureSpec& spec, const std::string& name = "");

		virtual void SetData(void* data, int index, uint32_t size, int level = 0) = 0;
		virtual void BindImage(uint32_t unit, int index, bool read, bool write, int level = 0) const = 0;

		[[nodiscard]] TextureType GetType() const override { return TextureType::Texture2DArray; }
	};

	class Texture3D : public Texture
	{
	public:
		static Ref<Texture2DArray> APEX_API Create(uint32_t width, uint32_t height, uint32_t depth, const TextureSpec& spec, const std::string& name = "");
	};


	struct TextureDesc
	{
		std::string_view name;												// 16 bytes
		std::vector<fs::path> srcPaths;										// 32 bytes

		// Non-owning pointer to texture data. Must be valid until Create() is called.
		void* data = nullptr;												// 8 bytes
		uint32_t dataSize = 0;												// 4 bytes
		uint32_t width = 0, height = 0;										// 8 bytes
		union { uint32_t depth = 0; uint32_t count; };						// 4 bytes
		union { uint32_t mipLevels = 0; uint32_t samples; };				// 4 bytes 

		TextureType type;													// 1 byte
		TextureAccessFormat accessFormat;									// 1 byte
		TextureInternalFormat internalFormat;								// 1 byte
		TextureDataType dataType;											// 1 byte
		TextureFiltering filtering = TextureFiltering::BILINEAR;			// 1 byte
		TextureWrapMode wrapMode[3] = { TextureWrapMode::CLAMP_TO_EDGE };	// 3 bytes
		
		glm::vec4 borderColor = {};											// 16 bytes
	};
	constexpr size_t SIZE_TEXTUREDESC = sizeof(TextureDesc);
	constexpr size_t SIZE_VECTOR = sizeof(std::vector<std::string_view>);
	constexpr size_t SIZE_PATH = sizeof(fs::path);
	constexpr size_t SIZE_VEC4 = sizeof(glm::vec4);



}
