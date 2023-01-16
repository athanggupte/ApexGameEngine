#pragma once

#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include <glad/glad.h>

namespace fs = std::filesystem;

namespace Apex {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		explicit OpenGLTexture2D(const TextureDesc& desc);
		OpenGLTexture2D(const fs::path& path, bool useSRGB, bool useHDR, TextureFiltering filtering);
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpec& spec, std::string name);
		~OpenGLTexture2D() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }
		[[nodiscard]] std::string_view GetName() const override { return m_Name; }

		void Bind(uint32_t slot = 0) const override;
		void BindImage(uint32_t unit, bool read, bool write, int level) const override;
		
		// virtual void Resize(uint32_t width, uint32_t height) override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void SetData(void* data, uint32_t size, int level) override;
		void GenerateMipmap() const override;
	private:
		std::string_view m_Name;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
		TextureFiltering m_Filtering;
		uint32_t m_PixelSize;
	};

	class OpenGLTexture2DMS : public Texture2DMS
	{
	public:
		explicit OpenGLTexture2DMS(const TextureDesc& desc);
		OpenGLTexture2DMS(uint32_t width, uint32_t height, const TextureSpec& spec, uint32_t samples, std::string name);
		~OpenGLTexture2DMS() override;

		[[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Height; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }

		[[nodiscard]] std::string_view GetName() const override { return m_Name; }

		void Bind(uint32_t slot = 0) const override;
		
		// virtual void Resize(uint32_t width, uint32_t height) override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void SetData(void* data, uint32_t size, int level) override;
		void GenerateMipmap() const override;
	private:
		std::string_view m_Name;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
		uint32_t m_PixelSize;
		uint32_t m_Samples;
	};

	class OpenGLTextureCubemap : public TextureCubemap
	{
	public:
		explicit OpenGLTextureCubemap(const TextureDesc& desc);
		OpenGLTextureCubemap(const std::array<fs::path, 6>& paths, bool useHDR, TextureFiltering filtering) [[deprecated]];
		OpenGLTextureCubemap(uint32_t size, const TextureSpec& spec, const std::string& name);
		~OpenGLTextureCubemap() override;
		[[nodiscard]] uint32_t GetWidth() const override { return m_Size; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_Size; }
		[[nodiscard]] uint32_t GetID() const override { return m_RendererID; }
		[[nodiscard]] std::string_view GetName() const override { return m_Name; }

		void Bind(uint32_t slot) const override;

		void SetData(void* data, int faceIndex, uint32_t size, int level) override;
		void Invalidate(uint32_t width, uint32_t height) override;
		void GenerateMipmap() const override;

	protected:
		void SetData(void* data, uint32_t size, int level) override {}

	private:
		std::string_view m_Name;
		uint32_t m_Size = 0;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
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
		[[nodiscard]] std::string_view GetName() const override { return m_Name; }

		void Bind(uint32_t slot = 0) const override;
		void BindImage(uint32_t unit, int index, bool read, bool write, int level) const override;
		
		void Invalidate(uint32_t width, uint32_t height) override;
		void SetData(void* data, int index, uint32_t size, int level) override;
		void GenerateMipmap() const override;
		
	protected:
		void SetData(void* data, uint32_t size, int level) override {}

	private:
		std::string_view m_Name;
		uint32_t m_Width, m_Height;
		uint32_t m_Count;
		uint32_t m_RendererID = 0;
		GLenum m_InternalFormat, m_AccessFormat, m_DataType;
		TextureFiltering m_Filtering;
		uint32_t m_PixelSize;
	};

}
