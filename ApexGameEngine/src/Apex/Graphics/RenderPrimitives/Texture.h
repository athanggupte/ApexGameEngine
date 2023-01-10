#pragma once

namespace Apex {
	struct TextureDesc;

	enum class TextureAccessFormat
	{
		RED, 		//I_RED,
		RG, 		//I_RG,
		RGB, BGR, 	//I_RGB, I_BGR,
		RGBA, BGRA, //I_RGBA, I_BGRA

		DEPTH,
		STENCIL,
	};
	
	enum class TextureInternalFormat
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
	
	enum class TextureDataType
	{
		BYTE, UBYTE,
		SHORT, USHORT,
		INT, UINT,
		FLOAT
	};

	enum class TextureFiltering
	{
		NEAREST,
		LINEAR,
		BILINEAR,
		TRILINEAR,

		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		NEAREST_MIPMAP_NEAREST,
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
		TextureFiltering filtering = TextureFiltering::BILINEAR;
	};
	
	// Default TextureSpecs
	inline static constexpr TextureSpec SimpleTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE };
	inline static constexpr TextureSpec SRGBTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::SRGBA8, TextureDataType::UBYTE };
	inline static constexpr TextureSpec HDRTextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT };
	inline static constexpr TextureSpec EnvironmentMapSpec{ TextureAccessFormat::RGB, TextureInternalFormat::RGB16, TextureDataType::FLOAT };

	enum class TextureType
	{
		Texture2D,
		TextureCubemap,
		//TextureDepth2D,
		Texture2DMS,

		Texture3D,
		Texture2DArray,
		TextureCubemapArray,
	};

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

		[[nodiscard]] virtual const std::string& GetName() const = 0;
		
		virtual void SetRows(uint32_t rows) { this->m_NumberOfRows = rows; this->m_MaxIndex = rows * rows; }
		virtual uint32_t GetRows() { return this->m_NumberOfRows; }
		virtual uint32_t GetMaxIndex() { return this->m_MaxIndex; }
		//[[nodiscard]] virtual TextureSpec GetSpec() const = 0;
		[[nodiscard]] virtual TextureType GetType() const = 0;

		static Ref<Texture> APEX_API Create(TextureDesc desc);

	protected:
		uint32_t m_NumberOfRows = 1;// , n_NumberOfCols = 1;
		uint32_t m_MaxIndex = 1;
		//TextureSpec m_Specification;
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

#ifdef SEPARATE_DEPTH_CLASS
	class TextureDepth2D : public Texture
	{
	public:
		static Ref<TextureDepth2D> APEX_API Create(uint32_t width = 1024, uint32_t height = 1024);

		[[nodiscard]] TextureType GetType() const override { return TextureType::TextureDepth2D; }
	};
#endif

//#define IMAGE_STORE_CLASS
#ifdef IMAGE_STORE_CLASS
	class ImageStore2D : public Texture2D
	{
		static Ref<ImageStore2D> Create(uint32_t width, uint32_t height, const std::string& name);

		virtual void BindImage(uint32_t unit, bool read, bool write) const = 0;
	};
#endif

	struct TextureDesc
	{
		fs::path sourceFile;
		void* data = nullptr; // Non-owning pointer to texture data. Must be valid until Create() is called.
		uint32_t dataSize;
		uint32_t width = 0, height = 0;
		TextureType type;
		TextureSpec spec;
		bool isHDR = false;
		bool isSRGB = false;
		int mipBaseLevel = 0;
		int mipMaxLevel;
	};


}
