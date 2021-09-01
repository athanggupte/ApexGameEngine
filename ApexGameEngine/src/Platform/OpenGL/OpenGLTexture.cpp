#include "apex_pch.h"
#include "OpenGLTexture.h"

#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/VFS.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Apex {

	using AF = TextureAccessFormat;
	using IF = TextureInternalFormat;
	using DT = TextureDataType;
	
	inline static GLenum GetOpenGLAccessFormat(TextureAccessFormat accessFormat)
	{
		switch (accessFormat)
		{
			case AF::RED	:	return GL_RED;
			//case AF::I_RED	:	return GL_RED_INTEGER;
			case AF::RG		:	return GL_RG;
			//case AF::I_RG	:	
			case AF::RGB	:	return GL_RGB;
			case AF::BGR	:	return GL_BGR;
			//case AF::I_RGB	:	
			//case AF::I_BGR	:	
			case AF::RGBA	:	return GL_RGBA;
			case AF::BGRA	:	return GL_BGRA;
			//case AF::I_RGBA	:	
			//case AF::I_BGRA	:	
		}
	}
	
#define GET_GL_INTERNAL_FORMAT(internalFormat, dataTypeSuffix) GL_##internalFormat##dataTypeSuffix
	
	inline static GLenum GetOpenGLInternalFormat(TextureInternalFormat internalFormat, TextureDataType dataType)
	{
		switch (dataType)
		{
			case DT::BYTE: 
			case DT::UBYTE:
			case DT::SHORT:
			case DT::USHORT:
			{
				switch (internalFormat)
				{
					case IF::R8: return GL_R8;
					case IF::RG8: return GL_RG8;
					case IF::RGB8: return GL_RGB8;
					case IF::RGBA8: return GL_RGBA8;
					
					case IF::R16: return GL_R16;
					case IF::RG16: return GL_RG16;
					case IF::RGB16: return GL_RGB16;
					case IF::RGBA16: return GL_RGBA16;
				}
			}
			case DT::INT: {
				switch (internalFormat)
				{
					case IF::R8: return GL_R8I;
					case IF::RG8: return GL_RG8I;
					case IF::RGB8: return GL_RGB8I;
					case IF::RGBA8: return GL_RGBA8I;
					
					case IF::R16: return GL_R16I;
					case IF::RG16: return GL_RG16I;
					case IF::RGB16: return GL_RGB16I;
					case IF::RGBA16: return GL_RGBA16I;
					
					case IF::R32: return GL_R32I;
					case IF::RG32: return GL_RG32I;
					case IF::RGB32: return GL_RGB32I;
					case IF::RGBA32: return GL_RGBA32I;
				}
			}
			case DT::UINT: {
				switch (internalFormat)
				{
					case IF::R8: return GL_R8UI;
					case IF::RG8: return GL_RG8UI;
					case IF::RGB8: return GL_RGB8UI;
					case IF::RGBA8: return GL_RGBA8UI;
					
					case IF::R16: return GL_R16UI;
					case IF::RG16: return GL_RG16UI;
					case IF::RGB16: return GL_RGB16UI;
					case IF::RGBA16: return GL_RGBA16UI;
					
					case IF::R32: return GL_R32UI;
					case IF::RG32: return GL_RG32UI;
					case IF::RGB32: return GL_RGB32UI;
					case IF::RGBA32: return GL_RGBA32UI;
				}
			}
			case DT::FLOAT: {
				switch (internalFormat)
				{
					case IF::R16: return GL_R16F;
					case IF::RG16: return GL_RG16F;
					case IF::RGB16: return GL_RGB16F;
					case IF::RGBA16: return GL_RGBA16F;
					
					case IF::R32: return GL_R32F;
					case IF::RG32: return GL_RG32F;
					case IF::RGB32: return GL_RGB32F;
					case IF::RGBA32: return GL_RGBA32F;
				}
			}
		}
	}
	
	inline static GLenum GetOpenGLDataType(TextureDataType dataType)
	{
		switch (dataType)
		{
			case DT::BYTE: return GL_BYTE;
			case DT::UBYTE: return GL_UNSIGNED_BYTE;
			case DT::SHORT: return GL_SHORT;
			case DT::USHORT: return GL_UNSIGNED_SHORT;
			case DT::INT: return GL_INT;
			case DT::UINT: return GL_UNSIGNED_INT;
			case DT::FLOAT: return GL_FLOAT;
		}
	}
	
	inline static uint32_t GetOpenGLPixelSize(TextureInternalFormat internalFormat)
	{
		return (((uint32_t)internalFormat & 0xf0) >> 4) * (1 << ((uint32_t)internalFormat & 0x0f));
	}
	
	//////////////////////////////////////////////////////////////////////
	/*---------------------------Texture 2D-----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpec& spec, const std::string& name)
		: m_Width(width), m_Height(height), m_Specification(spec)
	{
		m_AccessFormat = GetOpenGLAccessFormat(spec.accessFormat);
		m_InternalFormat = GetOpenGLInternalFormat(spec.internalFormat, spec.dataType);
		m_DataType = GetOpenGLDataType(spec.dataType);
		m_PixelSize = GetOpenGLPixelSize(spec.internalFormat);
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_AccessFormat, m_DataType, nullptr);
		
		if(!name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	OpenGLTexture2D::OpenGLTexture2D(const std::string & path, bool useHDR)
		: m_Path(path), m_Mutable(false)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		void *data = nullptr;
		
		std::string filepath = "";
		auto file = FileSystem::GetFileIfExists(path);
		if (file)
			filepath += file->GetPhysicalPath();
		else
			APEX_CORE_ERROR("Texture file {} not found!", path);
		
		if (useHDR) {
			data = (float*)stbi_loadf(filepath.c_str(), &width, &height, &channels, 0);
		} else {
			data = (stbi_uc*)stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		}
		APEX_CORE_ASSERT(data, std::string("Failed to load image : " + path));

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, accessFormat = 0, dataType = 0;

		if (channels == 1) {
			internalFormat = useHDR ? GL_R16F : GL_R8;
			accessFormat = GL_RED;
		}
		else if (channels == 3) {
			internalFormat = useHDR ? GL_RGB16F : GL_RGB8;
			accessFormat = GL_RGB;
		}
		else if (channels == 4) {
			internalFormat = useHDR ? GL_RGBA16F : GL_RGBA8;
			accessFormat = GL_RGBA;
		}
		
		APEX_CORE_ASSERT(internalFormat && accessFormat, "Image format not supported : " + path + "\nChannels : " + std::to_string(channels));
		
		m_AccessFormat = accessFormat;
		m_InternalFormat = internalFormat;
		m_DataType = useHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, data);

		stbi_image_free(data);
		
		auto name = Utils::GetFilename(path);
		glObjectLabel(GL_TEXTURE, m_RendererID, -1, name.c_str());
	}
	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}	

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		APEX_CORE_ASSERT(size == m_PixelSize * m_Width * m_Height, "Size of data not equal to size of texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, data);
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		if (!m_Mutable)
			APEX_CORE_ERROR("Cannot resize immutable texture!");
		
		m_Width = width;
		m_Height = height;
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_AccessFormat, m_DataType, nullptr);
	}

	void OpenGLTexture2D::BindImage(uint32_t unit, bool read, bool write) const
	{
		APEX_CORE_ASSERT(read || write, "Image should be atleast readable or writable");
		glBindImageTexture(unit, m_RendererID, 0, GL_FALSE, 0, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY, m_InternalFormat);
		Renderer::SetImageAccessBit();
	}
	
	//////////////////////////////////////////////////////////////////////
	/*------------------------Texture 2D HDR----------------------------*/
	//////////////////////////////////////////////////////////////////////
#ifdef SEPARATE_HDR
	OpenGLTexture2D_HDR::OpenGLTexture2D_HDR(uint32_t width, uint32_t height, const std::string& name)
		: m_Width(width), m_Height(height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, GL_RGBA16F, m_Width, m_Height);
		
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
	
	void OpenGLTexture2D_HDR::SetData(void* data, uint32_t size)
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
#endif

	//////////////////////////////////////////////////////////////////////
	/*-----------------------Texture Depth 2D---------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTextureDepth2D::OpenGLTextureDepth2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
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

	void OpenGLTextureDepth2D::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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
