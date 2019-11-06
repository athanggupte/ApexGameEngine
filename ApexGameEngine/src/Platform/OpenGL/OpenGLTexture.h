#pragma once

#include "Apex/Renderer/Texture.h"

namespace Apex {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D();
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		
		virtual uint32_t GetID() const override;

		virtual void Bind(uint32_t slot = 0) const override;

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};

	class OpenGLTexture2D_HDR : public Texture2D_HDR
	{
	public:
		OpenGLTexture2D_HDR();
		OpenGLTexture2D_HDR(const std::string& path);
		virtual ~OpenGLTexture2D_HDR();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		virtual uint32_t GetID() const override;

		virtual void Bind(uint32_t slot = 0) const override;

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
	};

}