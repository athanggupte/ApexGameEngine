#pragma once

namespace Apex {
	
	enum class TextureAccessFormat
	{
		RED, 		//I_RED,
		RG, 		//I_RG,
		RGB, BGR, 	//I_RGB, I_BGR,
		RGBA, BGRA, //I_RGBA, I_BGRA
	};
	
	enum class TextureInternalFormat
	{
		R8 = 0x10, R16, R32,
		RG8 = 0x20, RG16, RG32,
		RGB8 = 0x30, RGB16, RGB32,
		RGBA8 = 0x40, RGBA16, RGBA32,
		SRGB8 = 0x50, 
		SRGBA8 = 0x60
	};
	
	enum class TextureDataType
	{
		BYTE, UBYTE,
		SHORT, USHORT,
		INT, UINT,
		FLOAT
	};
	
	/*enum class Parameters
	{
		FILTER_MIN, FILTER_MAG,
		MIPMAP_LEVEL_MIN, MIPMAP_LEVEL_MAX,
		LOD_MIN, LOD_MAX,
		WRAP_U, WRAP_V, WRAP_W
	};*/
	
	struct TextureSpec
	{
		TextureAccessFormat accessFormat;
		TextureInternalFormat internalFormat;
		TextureDataType dataType;
	};
	
	// Default TextureSpecs
	inline static const TextureSpec SimpleTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE };
	inline static const TextureSpec HDRTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT };
	
	
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetID() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		
		virtual const std::string& GetPath() const = 0;
		
		virtual void SetRows(uint32_t rows) { this->m_NumberOfRows = rows; this->m_MaxIndex = rows * rows; }
		virtual uint32_t GetRows() { return this->m_NumberOfRows; }
		virtual uint32_t GetMaxIndex() { return this->m_MaxIndex; }
	protected:
		uint32_t m_NumberOfRows = 1;// , n_NumberOfCols = 1;
		uint32_t m_MaxIndex = 1;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path, bool useHDR = false);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, const TextureSpec& spec, const std::string& name = "");

		virtual void BindImage(uint32_t unit, bool read, bool write) const = 0;
		virtual const TextureSpec& GetSpec() const = 0;
	};

//#define SEPARATE_HDR
#ifdef SEPARATE_HDR
	class Texture2D_HDR : public Texture2D
	{
	public:
		static Ref<Texture2D_HDR> Create(const std::string& path);
		static Ref<Texture2D_HDR> Create(uint32_t width = 1280, uint32_t height = 720, const std::string& name = "");

		virtual void BindImage(uint32_t unit, bool read, bool write) const = 0;
		//void BindImage(uint32_t unit, bool read, bool write) const override { APEX_CORE_ERROR("BindImage not yet supported for HDR textures"); }
	};
#endif

	class TextureDepth2D : public Texture
	{
	public:
		static Ref<TextureDepth2D> Create(uint32_t width = 1024, uint32_t height = 1024);
	};

//#define IMAGE_STORE_CLASS
#ifdef IMAGE_STORE_CLASS
	class ImageStore2D : public Texture2D
	{
		static Ref<ImageStore2D> Create(uint32_t width, uint32_t height, const std::string& name);

		virtual void BindImage(uint32_t unit, bool read, bool write) const = 0;
	};
#endif
}
