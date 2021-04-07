#pragma once

#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include <glad/glad.h>

namespace Apex {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path, bool useHDR);
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpec& spec, const std::string& name);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		
		inline virtual uint32_t GetID() const override { return m_RendererID; }

		inline virtual const std::string& GetPath() const { return m_Path; }

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImage(uint32_t unit, bool read, bool write) const override;
		
		virtual void SetData(void* data, uint32_t size) override;
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
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

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImage(uint32_t unit, bool read, bool write) const override;
		
	protected:
		virtual void SetData(void* data, uint32_t size) override {}
		
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};
#endif

	class OpenGLTextureDepth2D : public TextureDepth2D
	{
	public:
		OpenGLTextureDepth2D();
		virtual ~OpenGLTextureDepth2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		inline virtual uint32_t GetID() const override { return m_RendererID; }

		virtual void Bind(uint32_t slot = 0) const override;
	protected:
		virtual void SetData(void* data, uint32_t size) override {}
		
	private:
		virtual const std::string& GetPath() const { return m_Path; }

	private:
		const std::string m_Path = "<N/A>";
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};
	
#ifdef IMAGE_STORE_CLASS
	class OpenGLImageStore2D : public ImageStore2D
	{
	public:
		OpenGLImageStore2D(uint32_t width, uint32_t height, const std::string& name);
		virtual ~OpenGLImageStore2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		inline virtual uint32_t GetID() const override { return m_RendererID; }

		virtual void Bind(uint32_t slot) const override;
		virtual void BindImage(uint32_t unit, bool read, bool write) const override;

	private:
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};
#endif
}
