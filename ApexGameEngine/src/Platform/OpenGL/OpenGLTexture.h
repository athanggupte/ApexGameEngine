#pragma once

#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include <glad/glad.h>

namespace fs = std::filesystem;

namespace Apex {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const fs::path& path, bool useSRGB, bool useHDR, TextureFiltering filtering);
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpec& spec, std::string name);
		~OpenGLTexture2D() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }
		[[nodiscard]] const std::string& GetName() const override { return m_Name; }

		void Bind(uint32_t slot = 0) const override;
		void BindImage(uint32_t unit, bool read, bool write) const override;
		
		// virtual void Resize(uint32_t width, uint32_t height) override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void SetData(void* data, uint32_t size) override;
		void GenerateMipmap() const override;
	private:
		std::string m_Name;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
		TextureFiltering m_Filtering;
		uint32_t m_PixelSize;
	};

	class OpenGLTexture2DMS : public Texture2DMS
	{
	public:
		OpenGLTexture2DMS(uint32_t width, uint32_t height, const TextureSpec& spec, uint32_t samples, std::string name);
		~OpenGLTexture2DMS() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }

		[[nodiscard]] const std::string& GetName() const override { return m_Name; }

		void Bind(uint32_t slot = 0) const override;
		
		// virtual void Resize(uint32_t width, uint32_t height) override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void SetData(void* data, uint32_t size) override;
		void GenerateMipmap() const override;
	private:
		std::string m_Name;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
		uint32_t m_PixelSize;
	};

	class OpenGLTextureCubemap : public TextureCubemap
	{
	public:
		OpenGLTextureCubemap(const std::array<fs::path, 6>& paths, bool useHDR, TextureFiltering filtering);
		OpenGLTextureCubemap(uint32_t size, const TextureSpec& spec, const std::string& name);
		~OpenGLTextureCubemap() override;
		[[nodiscard]] uint32_t GetWidth() const override { return m_Size; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Size; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }
		[[nodiscard]] const std::string& GetName() const override { return m_Name; }

		void Bind(uint32_t slot) const override;

		void SetData(void* data, int faceIndex, uint32_t size) override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void GenerateMipmap() const override;

	protected:
		void SetData(void* data, uint32_t size) override {}

	private:
		std::string m_Name;
		uint32_t m_Size = 0;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_DataType;
		TextureFiltering m_Filtering;
		uint32_t m_PixelSize;
	};

	class OpenGLTexture2DArray : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(uint32_t width, uint32_t height, uint32_t count, const TextureSpec& spec, std::string name);
		~OpenGLTexture2DArray() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }
		[[nodiscard]] const std::string& GetName() const override { return m_Name; }

		void Bind(uint32_t slot = 0) const override;
		void BindImage(uint32_t unit, int index, bool read, bool write) const override;
		
		void Invalidate(uint32_t width, uint32_t height) override;
		void SetData(void* data, int index, uint32_t size) override;
		void GenerateMipmap() const override;
		
	protected:
		void SetData(void* data, uint32_t size) override {}
	private:
		std::string m_Name;
		uint32_t m_Width, m_Height;
		uint32_t m_Count;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
		TextureFiltering m_Filtering;
		uint32_t m_PixelSize;
	};

#ifdef SEPARATE_HDR
	class OpenGLTexture2D_HDR : public Texture2D_HDR
	{
	public:
		OpenGLTexture2D_HDR(uint32_t width, uint32_t height, const std::string& name);
		OpenGLTexture2D_HDR(const std::string& path);
		virtual ~OpenGLTexture2D_HDR();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		inline virtual uint32_t GetID() const override { return m_RendererID; }

		inline virtual const std::string& GetPath() const { return m_Path; }

		virtual void BindTextures(uint32_t slot = 0) const override;
		virtual void BindImage(uint32_t unit, bool read, bool write) const override;
	protected:
		virtual void SetData(void* data, uint32_t size) override {}
		
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
	};
#endif

#ifdef SEPARATE_DEPTH_CLASS
  	class OpenGLTextureDepth2D : public TextureDepth2D
	{
	public:
		OpenGLTextureDepth2D(uint32_t width, uint32_t height);
		~OpenGLTextureDepth2D() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }

		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }

		void Bind(uint32_t slot = 0) const override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void GenerateMipmap() const override;

	protected:
		void SetData(void* data, uint32_t size) override {}
		
	private:
		[[nodiscard]] const std::string& GetPath() const override { return m_Path; }
	private:
		const std::string m_Path = "<N/A>";
		uint32_t m_Width, m_Height;
		TextureFiltering m_Filtering;
		uint32_t m_RendererID = 0;
	};
#endif


#ifdef IMAGE_STORE_CLASS
	class OpenGLImageStore2D : public ImageStore2D
	{
	public:
		OpenGLImageStore2D(uint32_t width, uint32_t height, const std::string& name);
		virtual ~OpenGLImageStore2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		inline virtual uint32_t GetID() const override { return m_RendererID; }

		virtual void BindTextures(uint32_t slot) const override;
		virtual void BindImage(uint32_t unit, bool read, bool write) const override;

	private:
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
	};
#endif
}
