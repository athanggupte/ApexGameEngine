#pragma once

namespace Apex {
	
	struct TextureSpec
	{
		enum class AccessFormat
		{
			RED, I_RED,
			RG, I_RG,
			RGB, BGR, I_RGB, I_BGR,
			RGBA, BGRA, I_RGBA, I_BGRA
		};
		
		enum class InternalFormat
		{
			R8, R16, 		//R16F, R32F,
			RG8, RG16, 		//RG16F, RG32F,
			RGB8, 			//RGB16F, RGB32F,
			RGBA8, RGBA16, 	//RGBA16F, RGBA32F 
		};
		
		enum class FormatDataType
		{
			BYTE, UBYTE, INT, UINT, FLOAT
		}
	};
	
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetID() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual void SetRows(uint32_t rows) { this->m_NumberOfRows = rows; this->m_MaxIndex = rows * rows; }
		virtual uint32_t GetRows() { return this->m_NumberOfRows; }
		virtual uint32_t GetMaxIndex() { return this->m_MaxIndex; }
		
		virtual void SetData(void* data, uint32_t size) = 0;
	private:
		uint32_t m_NumberOfRows = 1;// , n_NumberOfCols = 1;
		uint32_t m_MaxIndex = 1;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(uint32_t width = 1280, uint32_t height = 720, const std::string& name = "");

		virtual void BindImage(uint32_t unit, bool read, bool write) const = 0;
	};

	class Texture2D_HDR : public Texture2D
	{
	public:
		static Ref<Texture2D_HDR> Create(const std::string& path);
		static Ref<Texture2D_HDR> Create(uint32_t width = 1280, uint32_t height = 720, const std::string& name = "");

		virtual void BindImage(uint32_t unit, bool read, bool write) const = 0;
		//void BindImage(uint32_t unit, bool read, bool write) const override { APEX_CORE_ERROR("BindImage not yet supported for HDR textures"); }
	};

	class TextureDepth2D : public Texture
	{
	public:
		static Ref<TextureDepth2D> Create();
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
