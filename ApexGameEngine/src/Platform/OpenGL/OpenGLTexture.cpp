#include "apex_pch.h"
#include "OpenGLTexture.h"

#include "Apex/Graphics/Renderer/Renderer.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Apex {

	//////////////////////////////////////////////////////////////////////
	/*---------------------------Texture 2D-----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const std::string& name)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);
		
		//std::vector<std::tuple<GLubyte, GLubyte, GLubyte, GLubyte>> data(width * height);
		//data.assign(width * height, { 255, 252, 3, 232 });
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		
		if(!name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	OpenGLTexture2D::OpenGLTexture2D(const std::string & path)
		: m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		APEX_CORE_ASSERT(data, std::string("Failed to load image : " + path));

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		if (channels == 1) {
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}

		APEX_CORE_ASSERT(internalFormat && dataFormat, "Image format not supported : " + path + "\nChannels : " + std::to_string(channels));

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}	

	void OpenGLTexture2D::BindImage(uint32_t unit, bool read, bool write) const
	{
		APEX_CORE_ASSERT(read || write, "Image should be atleast readable or writable");
		glBindImageTexture(unit, m_RendererID, 0, GL_FALSE, 0, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY, GL_RGBA8);
		Renderer::SetImageAccessBit();
	}

	//////////////////////////////////////////////////////////////////////
	/*------------------------Texture 2D HDR----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTexture2D_HDR::OpenGLTexture2D_HDR(uint32_t width, uint32_t height, const std::string& name)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, GL_RGBA16F, m_Width, m_Height);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_FLOAT, nullptr);
		
		if (!name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLTexture2D_HDR::OpenGLTexture2D_HDR(const std::string & path)
		: m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		APEX_CORE_ASSERT(data, std::string("Failed to load image : " + path));

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;

		
		if (channels == 3) {
			internalFormat = GL_RGB16F;
			dataFormat = GL_RGB;
		}
		else if (channels == 4) {
			internalFormat = GL_RGBA16F;
			dataFormat = GL_RGBA;
		}

		APEX_CORE_ASSERT(internalFormat && dataFormat, "Image format not supported");

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
		//glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_FLOAT, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D_HDR::~OpenGLTexture2D_HDR()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D_HDR::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D_HDR::BindImage(uint32_t unit, bool read, bool write) const
	{
		APEX_CORE_ASSERT(read || write, "Image should be atleast readable or writable");
		glBindImageTexture(unit, m_RendererID, 0, GL_FALSE, 0, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY, GL_RGBA16F);
		Renderer::SetImageAccessBit();
	}

	//////////////////////////////////////////////////////////////////////
	/*-----------------------Texture Depth 2D---------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTextureDepth2D::OpenGLTextureDepth2D()
		: m_Width(2048), m_Height(2048)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	OpenGLTextureDepth2D::~OpenGLTextureDepth2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureDepth2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	//////////////////////////////////////////////////////////////////////
	/*------------------------Image Store 2D----------------------------*/
	//////////////////////////////////////////////////////////////////////
#ifdef IMAGE_STORE_CLASS
	OpenGLImageStore2D::OpenGLImageStore2D(uint32_t width, uint32_t height, const std::string& name)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		
		//glTextureStorage2D(m_RendererID, 1, GL_RGBA8, m_Width, m_Height);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		if (!name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLImageStore2D::~OpenGLImageStore2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLImageStore2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLImageStore2D::BindImage(uint32_t unit, bool read, bool write) const
	{
		APEX_CORE_ASSERT(read || write, "Image should be atleast readable or writable");
		glBindImageTexture(unit, m_RendererID, 0, GL_FALSE, 0, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY, GL_RGB8);
		Renderer::SetImageAccessBit();
	}
#endif
}