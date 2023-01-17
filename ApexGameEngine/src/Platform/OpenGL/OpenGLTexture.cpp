#include "apex_pch.h"
#include "OpenGLTexture.h"

#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/FileSystem.h"
#include "Util/ScopeGuard.hpp"

#include <glad/glad.h>
#include <stb_image.h>

namespace Apex
{
	using AF = TextureAccessFormat;
	using IF = TextureInternalFormat;
	using DT = TextureDataType;
	
	constexpr static GLenum GetOpenGLAccessFormat(TextureAccessFormat accessFormat)
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
			case AF::DEPTH			:	return GL_DEPTH_COMPONENT;
			case AF::STENCIL		:	return GL_STENCIL_INDEX;
		}
		return 0;
	}

#define GET_GL_INTERNAL_FORMAT(internalFormat, dataTypeSuffix) GL_##internalFormat##dataTypeSuffix
	
	constexpr static GLenum GetOpenGLInternalFormat(TextureInternalFormat internalFormat, TextureDataType dataType)
	{
		if (internalFormat == IF::SRGB8) return GL_SRGB8;
		if (internalFormat == IF::SRGBA8) return GL_SRGB8_ALPHA8;

		if (internalFormat == IF::DEPTH16) return GL_DEPTH_COMPONENT16;
		if (internalFormat == IF::DEPTH24) return GL_DEPTH_COMPONENT24;
		if (internalFormat == IF::DEPTH32) return GL_DEPTH_COMPONENT32F;

		if (internalFormat == IF::D24S8) return GL_DEPTH24_STENCIL8;
		if (internalFormat == IF::D32S8) return GL_DEPTH32F_STENCIL8;

		if (internalFormat == IF::STENCIL8) return GL_STENCIL_INDEX8;

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

					default: return GL_RGBA8;
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

					default: return GL_RGBA16I;
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

					default: return GL_RGBA16UI;
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

					default: return GL_RGBA16F;
				}
			}
		}
		return 0;
	}
	
	constexpr static GLenum GetOpenGLDataType(TextureDataType dataType)
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
		return 0;
	}

	constexpr static uint32_t GetNumChannels(TextureInternalFormat internal_format)
	{
		if (internal_format == IF::SRGB8) return 3;
		if (internal_format == IF::SRGBA8) return 4;
		if (internal_format >= IF::DEPTH16) return 1;
		return (static_cast<uint32_t>(internal_format) & 0xf0) >> 4;
	}

	constexpr static uint32_t GetOpenGLPixelSize(TextureInternalFormat internal_format)
	{
		if (internal_format == IF::SRGB8) return 3;
		if (internal_format == IF::SRGBA8) return 4;
		if (internal_format >= IF::DEPTH16) return 1;
		return ((static_cast<uint32_t>(internal_format) & 0xf0) >> 4) * (1 << (static_cast<uint32_t>(internal_format) & 0x0f));
	}

	template<TextureType type = TextureType::Texture2D>
	constexpr static std::pair<GLint, GLint> GetMinMagFilterPair(TextureFiltering filtering)
	{
		std::pair<GLint, GLint> ret;
		/*if constexpr (type == TextureType::Texture3D) {
			
		}*/
		switch (filtering)
		{
		case TextureFiltering::NEAREST:
			{
				ret.first = GL_NEAREST;
				ret.second = GL_NEAREST;
				break;
			}
		case TextureFiltering::LINEAR:
			{
				ret.first = GL_LINEAR;
				ret.second = GL_LINEAR;
				break;
			}
		case TextureFiltering::BILINEAR:
			{
				ret.first = GL_LINEAR;
				ret.second = GL_LINEAR;
				break;
			}
		case TextureFiltering::TRILINEAR:
			{
				ret.first = GL_LINEAR_MIPMAP_LINEAR;
				ret.second = GL_LINEAR;
				break;
			}
		case TextureFiltering::LINEAR_MIPMAP_NEAREST:
			{
				ret.first = GL_LINEAR_MIPMAP_NEAREST;
				ret.second = GL_LINEAR;
				break;
			}
		case TextureFiltering::NEAREST_MIPMAP_LINEAR:
			{
				ret.first = GL_NEAREST_MIPMAP_LINEAR;
				ret.second = GL_LINEAR;
				break;
			}
		case TextureFiltering::NEAREST_MIPMAP_NEAREST:
			{
				ret.first = GL_NEAREST_MIPMAP_NEAREST;
				ret.second = GL_LINEAR;
				break;
			}
		}
		return ret;
	}

	constexpr static bool UseMipmaps(TextureFiltering filtering)
	{
		switch (filtering)
		{
		case TextureFiltering::NEAREST:
		case TextureFiltering::LINEAR:
		case TextureFiltering::BILINEAR:
			return false;

		case TextureFiltering::TRILINEAR:
		case TextureFiltering::LINEAR_MIPMAP_NEAREST:
		case TextureFiltering::NEAREST_MIPMAP_LINEAR:
		case TextureFiltering::NEAREST_MIPMAP_NEAREST:
			return true;
		}
		return true;
	}

	constexpr static GLint GetOpenGLWrapMode(TextureWrapMode wrap_mode)
	{
		switch (wrap_mode)
		{
		case TextureWrapMode::REPEAT:			return GL_REPEAT;
		case TextureWrapMode::MIRRORED:			return GL_MIRRORED_REPEAT;
		case TextureWrapMode::CLAMP_TO_EDGE:	return GL_CLAMP_TO_EDGE;
		case TextureWrapMode::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
		}
		return 0;
	}

	constexpr static uint32_t log2(uint32_t n)
	{
		return (n > 1) ? 1 + log2(n >> 1) : 0;
	}

	constexpr static uint32_t GetNumMipLevels(uint32_t width, uint32_t height)
	{
		return std::max(1u, std::min(8u, log2(std::min(width, height))));
	}

	//////////////////////////////////////////////////////////////////////
	/*---------------------------Texture 2D-----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(const TextureDesc& desc)
		: m_Name(desc.name),
		  m_Width(desc.width), m_Height(desc.height),
		  m_InternalFormat(GetOpenGLInternalFormat(desc.internalFormat, desc.dataType)),
		  m_AccessFormat(GetOpenGLAccessFormat(desc.accessFormat)),
		  m_DataType(GetOpenGLDataType(desc.dataType)),
		  m_Filtering(desc.filtering),
		  m_PixelSize(GetOpenGLPixelSize(desc.internalFormat))
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLWrapMode(desc.wrapMode[0]));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLWrapMode(desc.wrapMode[1]));

		void* pixels;

		if (!desc.srcPaths.empty()) {
			int width, height, channels;
			stbi_set_flip_vertically_on_load(TRUE);

			std::vector<uint8_t> data = FileSystem::ReadFromFile(desc.srcPaths[0]);

			if (desc.dataType == TextureDataType::FLOAT)
				pixels = stbi_loadf_from_memory(data.data(), data.size(),
											&width, &height, &channels,
			                                GetNumChannels(desc.internalFormat));
			else
				pixels = stbi_load_from_memory(data.data(), data.size(),
											&width, &height, &channels,
			                                GetNumChannels(desc.internalFormat));

			APEX_CORE_ASSERT(pixels, fmt::format("Failed to load image : {0} | [stbi]: {1}", desc.srcPaths[0], stbi_failure_reason()));
			m_Width = width;
			m_Height = height;

			if (m_Name.empty())
				m_Name = TEXT(desc.srcPaths[0].string()).str();
		} else {
			pixels = desc.data;
		}

		ON_SCOPE_END { if (pixels && !desc.srcPaths.empty()) stbi_image_free(pixels); };

		APEX_CORE_ASSERT(m_Width > 0 && m_Height > 0, "Width and Height of texture must be 1 or greater!");

		m_MipLevels = desc.mipLevels ? desc.mipLevels : UseMipmaps(m_Filtering) ? GetNumMipLevels(m_Width, m_Height) : 1;
		glTextureStorage2D(m_RendererID, m_MipLevels, m_InternalFormat, m_Width, m_Height);

		if (pixels)
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, pixels);

		if(!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSpec& spec, std::string name)
		: m_Name(std::move(name)), m_Width(width), m_Height(height),
		  m_InternalFormat(GetOpenGLInternalFormat(spec.internalFormat, spec.dataType)),
		  m_AccessFormat(GetOpenGLAccessFormat(spec.accessFormat)),
		  m_DataType(GetOpenGLDataType(spec.dataType)), m_Filtering(spec.filtering),
		  m_PixelSize(GetOpenGLPixelSize(spec.internalFormat))
	{
		APEX_CORE_ASSERT(width > 0 && height > 0, "Width and Height of texture must be 1 or greater!");
		//m_Specification = spec;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		if (spec.accessFormat == AF::DEPTH) {
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_MipLevels = UseMipmaps(spec.filtering) ? GetNumMipLevels(width, height) : 1;
		glTextureStorage2D(m_RendererID, static_cast<int>(m_MipLevels), m_InternalFormat, static_cast<int>(m_Width), static_cast<int>(m_Height));

		if(!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTexture2D::OpenGLTexture2D(const fs::path& path, bool useSRGB, bool useHDR, TextureFiltering filtering)
		: m_Name(TEXT(path.string()).str())
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		void *pixels = nullptr;
		
		std::vector<uint8_t> data;
		if (const auto file = FileSystem::GetFileIfExists(path); file && file->OpenRead()) {
			data.resize(file->Size());
			file->Read(data.data(), data.size());
			file->Close();
		} else {
			APEX_CORE_CRITICAL("Could not open Texture file {}", file->GetPhysicalPath());
			return;
		}

		if (useHDR) {
			pixels = stbi_loadf_from_memory(data.data(), data.size(), &width, &height, &channels, 4);
			channels = 4;
		} else {
			pixels = stbi_load_from_memory(data.data(), data.size(), &width, &height, &channels, 0);
		}
		APEX_CORE_ASSERT(pixels, fmt::format("Failed to load image : {0} | [stbi]: {1}", path, stbi_failure_reason()));
		ON_SCOPE_END { stbi_image_free(pixels); };

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, accessFormat = 0;
		TextureInternalFormat apxInternalFormat = TextureInternalFormat::RGBA8;

		if (channels == 1) {
			accessFormat = GL_RED;
			internalFormat = useHDR ? GL_R16F : GL_R8;

			//m_Specification.accessFormat = TextureAccessFormat::RED;
			//m_Specification.internalFormat = useHDR ? TextureInternalFormat::R16 : TextureInternalFormat::R8;
			apxInternalFormat = useHDR ? TextureInternalFormat::R16 : TextureInternalFormat::R8;
		}
		else if (channels == 3) {
			accessFormat = GL_RGB;
			internalFormat = useSRGB ? GL_SRGB8 : useHDR ? GL_RGB16F : GL_RGB8;
			
			//m_Specification.accessFormat = TextureAccessFormat::RGB;
			//m_Specification.internalFormat = useSRGB ? TextureInternalFormat::SRGB8 : useHDR ? TextureInternalFormat::RGB16 : TextureInternalFormat::RGB8;
			apxInternalFormat = useSRGB ? TextureInternalFormat::SRGB8 : useHDR ? TextureInternalFormat::RGB16 : TextureInternalFormat::RGB8;
		}
		else if (channels == 4) {
			accessFormat = GL_RGBA;
			internalFormat = useSRGB ? GL_SRGB8_ALPHA8 : useHDR ? GL_RGBA16F : GL_RGBA8;
			
			//m_Specification.accessFormat = TextureAccessFormat::RGBA;
			//m_Specification.internalFormat = useSRGB ? TextureInternalFormat::SRGBA8 : useHDR ? TextureInternalFormat::RGBA16 : TextureInternalFormat::RGBA8;
			apxInternalFormat = useSRGB ? TextureInternalFormat::SRGBA8 : useHDR ? TextureInternalFormat::RGBA16 : TextureInternalFormat::RGBA8;
		}
		
		APEX_CORE_ASSERT(internalFormat && accessFormat, fmt::format("Image format not supported : {0}\nChannels : {1}", path, channels));
		
		m_AccessFormat = accessFormat;
		m_InternalFormat = internalFormat;
		m_DataType = useHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;
		//m_PixelSize = GetOpenGLPixelSize(m_Specification.internalFormat);
		m_PixelSize = GetOpenGLPixelSize(apxInternalFormat);
		m_Filtering = filtering;
		m_MipLevels = UseMipmaps(filtering) ? GetNumMipLevels(width, height) : 1;
		//m_Specification.dataType = useHDR ? TextureDataType::FLOAT : TextureDataType::UBYTE;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		glTextureStorage2D(m_RendererID, m_MipLevels, m_InternalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, pixels);

		glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), path.filename().string().c_str());
	}
	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}	

	void OpenGLTexture2D::BindImage(uint32_t unit, bool read, bool write, int level) const
	{
		APEX_CORE_ASSERT(read || write, "Image should be atleast readable or writable");
		glBindImageTexture(unit, m_RendererID, level, GL_FALSE, 0, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY, m_InternalFormat);
		//Renderer::SetImageAccessBit();
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size, int level)
	{
		APEX_CORE_ASSERT(size == m_PixelSize * m_Width * m_Height, "Size of data not equal to size of texture!");
		glTextureSubImage2D(m_RendererID, level, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, data);
	}
	
	void OpenGLTexture2D::GenerateMipmap() const
	{
		glGenerateTextureMipmap(m_RendererID);
	}

	void OpenGLTexture2D::Invalidate(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		glDeleteTextures(1, &m_RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);
		
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
	}

	//////////////////////////////////////////////////////////////////////
	/*---------------------Texture 2D Multisample-----------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTexture2DMS::OpenGLTexture2DMS(const TextureDesc& desc)
		: m_Name(desc.name),
		  m_Width(desc.width), m_Height(desc.height),
		  m_InternalFormat(GetOpenGLInternalFormat(desc.internalFormat, desc.dataType)),
		  m_AccessFormat(GetOpenGLAccessFormat(desc.accessFormat)),
		  m_DataType(GetOpenGLDataType(desc.dataType)),
		  m_PixelSize(GetOpenGLPixelSize(desc.internalFormat)),
		  m_Samples(desc.samples)
	{
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLWrapMode(desc.wrapMode[0]));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLWrapMode(desc.wrapMode[1]));

		void* pixels;

		if (!desc.srcPaths.empty()) {
			int width, height, channels;
			stbi_set_flip_vertically_on_load(TRUE);

			std::vector<uint8_t> data = FileSystem::ReadFromFile(desc.srcPaths[0]);

			if (desc.dataType == TextureDataType::FLOAT)
				pixels = stbi_loadf_from_memory(data.data(), data.size(),
											&width, &height, &channels,
			                                GetNumChannels(desc.internalFormat));
			else
				pixels = stbi_load_from_memory(data.data(), data.size(),
											&width, &height, &channels,
			                                GetNumChannels(desc.internalFormat));

			m_Width = width;
			m_Height = height;

			if (m_Name.empty())
				m_Name = TEXT(desc.srcPaths[0].string()).str();
		} else {
			pixels = desc.data;
		}

		ON_SCOPE_END { if (pixels && !desc.srcPaths.empty()) stbi_image_free(pixels); };
		APEX_CORE_ASSERT(pixels, fmt::format("Failed to load image : {0} | [stbi]: {1}", desc.srcPaths[0], stbi_failure_reason()));

		APEX_CORE_ASSERT(m_Width > 0 && m_Height > 0, "Width and Height of texture must be 1 or greater!");

		glTextureStorage2DMultisample(m_RendererID, m_Samples, m_InternalFormat, m_Width, m_Height, GL_TRUE);

		if (pixels)
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, pixels);

		if(!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTexture2DMS::OpenGLTexture2DMS(uint32_t width, uint32_t height, const TextureSpec& spec, uint32_t samples, std::string name)
		: m_Name(std::move(name)), m_Width(width), m_Height(height),
		  m_InternalFormat(GetOpenGLInternalFormat(spec.internalFormat, spec.dataType)),
		  m_AccessFormat(GetOpenGLAccessFormat(spec.accessFormat)),
		  m_DataType(GetOpenGLDataType(spec.dataType)),
		  m_PixelSize(GetOpenGLPixelSize(spec.internalFormat)),
	      m_Samples(samples)
	{
		APEX_CORE_ASSERT(width > 0 && height > 0, "Width and Height of texture must be 1 or greater!");
		//m_Specification = spec;

		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_RendererID);

		glTextureStorage2DMultisample(m_RendererID, m_Samples, m_InternalFormat, m_Width, m_Height, GL_TRUE);
		
		if(!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTexture2DMS::~OpenGLTexture2DMS()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DMS::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2DMS::Invalidate(uint32_t width, uint32_t height)
	{
		APEX_CORE_ASSERT(width > 0 && height > 0, "Width and Height of texture must be 1 or greater!");
		m_Width = width;
		m_Height = height;

		glDeleteTextures(1, &m_RendererID);

		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_RendererID);

		glTextureStorage2DMultisample(m_RendererID, m_Samples, m_InternalFormat, m_Width, m_Height, GL_TRUE);
		
		if(!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	void OpenGLTexture2DMS::SetData(void* data, uint32_t size, int level)
	{
		APEX_CORE_ASSERT(size == m_PixelSize * m_Width * m_Height, "Size of data not equal to size of texture!");
		glTextureSubImage2D(m_RendererID, level, 0, 0, m_Width, m_Height, m_AccessFormat, m_DataType, data);
	}

	void OpenGLTexture2DMS::GenerateMipmap() const
	{
		glGenerateTextureMipmap(m_RendererID);
	}
	
	//////////////////////////////////////////////////////////////////////
	/*-------------------------Texture Cubemap--------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTextureCubemap::OpenGLTextureCubemap(const TextureDesc& desc)
		: m_Name(desc.name), m_Size(desc.width),
		  m_InternalFormat(GetOpenGLInternalFormat(desc.internalFormat, desc.dataType)),
		  m_AccessFormat(GetOpenGLAccessFormat(desc.accessFormat)),
		  m_DataType(GetOpenGLDataType(desc.dataType)),
		  m_Filtering(desc.filtering),
		  m_PixelSize(GetOpenGLPixelSize(desc.internalFormat))
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLWrapMode(desc.wrapMode[0]));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLWrapMode(desc.wrapMode[1]));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GetOpenGLWrapMode(desc.wrapMode[2]));

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		if (desc.srcPaths.size() == 6) {
			for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
				int width, height, channels;
				stbi_set_flip_vertically_on_load(TRUE);
				void* pixels = nullptr;

				std::vector<uint8_t> data = FileSystem::ReadFromFile(desc.srcPaths[0]);

				if (desc.dataType == TextureDataType::FLOAT)
					pixels = stbi_loadf_from_memory(data.data(), data.size(),
												&width, &height, &channels,
				                                GetNumChannels(desc.internalFormat));
				else
					pixels = stbi_load_from_memory(data.data(), data.size(),
												&width, &height, &channels,
				                                GetNumChannels(desc.internalFormat));

				APEX_CORE_ASSERT(pixels, fmt::format("Failed to load image : {0} | [stbi]: {1}", desc.srcPaths[faceIndex], stbi_failure_reason()));
				ON_SCOPE_END { stbi_image_free(pixels); };

				APEX_CORE_ASSERT(width == height, "Cubemap textures must have equal width and height!");
				if (faceIndex == 0) {
					m_Size = width;
					m_MipLevels = UseMipmaps(m_Filtering) ? GetNumMipLevels(width, height) : 1;
					glTextureStorage2D(m_RendererID, m_MipLevels, m_InternalFormat, m_Size, m_Size);

					if (m_Name.empty())
						m_Name = TEXT(desc.srcPaths[0].string()).str();
				}
				APEX_CORE_ASSERT(m_Size == static_cast<uint32_t>(width), "All cubemap textures must have same width!");

				glTextureSubImage3D(m_RendererID, 0, 0, 0, faceIndex, m_Size, m_Size, 1, m_InternalFormat, m_DataType, pixels);
			}
		} else if (desc.data && desc.dataSize > 0 && desc.dataSize % (m_Size * m_Size) == 0) {
			m_MipLevels = UseMipmaps(m_Filtering) ? GetNumMipLevels(m_Size, m_Size) : 1;
			glTextureStorage2D(m_RendererID, m_MipLevels, m_InternalFormat, m_Size, m_Size);
			if (desc.dataSize % 6 == 0) {
				const uint32_t deltaOffset = desc.dataSize / 6;
				uint32_t offset = 0;
				for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
					const void* pixels = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(desc.data) + offset);
					glTextureSubImage3D(m_RendererID, 0, 0, 0, faceIndex, m_Size, m_Size, 1, m_InternalFormat, m_DataType, pixels);
					offset += deltaOffset;
				}
			} else {
				for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
					glTextureSubImage3D(m_RendererID, 0, 0, 0, faceIndex, m_Size, m_Size, 1, m_InternalFormat, m_DataType, desc.data);
				}
			}
		}

		if (!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTextureCubemap::OpenGLTextureCubemap(const std::array<std::filesystem::path, 6>& paths, bool useHDR, TextureFiltering filtering)
		: m_InternalFormat(useHDR ? GL_RGB16F : GL_RGB8),
		  m_AccessFormat(GL_RGB),
		  m_DataType(useHDR ? GL_FLOAT : GL_UNSIGNED_BYTE),
		  m_Filtering(filtering),
		  m_PixelSize(useHDR ? 6 : 3)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
			std::vector<uint8_t> tmp_data;
			void* pixels = nullptr;
			if (const auto file = FileSystem::GetFileIfExists(paths[faceIndex]); file && file->OpenRead()) {
				tmp_data.resize(file->Size());
				file->Read(tmp_data.data(), tmp_data.size());
				file->Close();
			} else {
				APEX_CORE_CRITICAL("Could not open Texture file {}", paths[faceIndex]);
				return;
			}

			int width, height, channels;
			if (useHDR) {
				pixels = stbi_loadf_from_memory(tmp_data.data(), tmp_data.size(), &width, &height, &channels, 3);
			} else {
				pixels = stbi_load_from_memory(tmp_data.data(), tmp_data.size(), &width, &height, &channels, 3);
			}
			APEX_CORE_ASSERT(pixels, fmt::format("Failed to load image : {0} | [stbi]: {1}", paths[faceIndex], stbi_failure_reason()));
			APEX_CORE_ASSERT(width == height, "Cubemap textures must have equal width and height!");
			if (m_Size == 0) {
				m_Size = width;
				m_MipLevels = UseMipmaps(filtering) ? GetNumMipLevels(width, height) : 1;
				glTextureStorage2D(m_RendererID, m_MipLevels, m_InternalFormat, m_Size, m_Size);
			}
			APEX_CORE_ASSERT(m_Size == width, "All cubemap textures must have same width!");

			glTextureSubImage3D(m_RendererID, 0, 0, 0, faceIndex, m_Size, m_Size, 1, m_AccessFormat, m_DataType, pixels);
		}
	}

	OpenGLTextureCubemap::OpenGLTextureCubemap(uint32_t size, const TextureSpec& spec, const std::string& name)
		: m_Name(name), m_Size(size),
		  m_InternalFormat(GetOpenGLInternalFormat(spec.internalFormat, spec.dataType)),
		  m_DataType(GetOpenGLDataType(spec.dataType)),
		  m_Filtering(spec.filtering),
		  m_PixelSize(GetOpenGLPixelSize(spec.internalFormat))
	{
		//m_Specification = spec;
		APEX_CORE_ASSERT(size > 0, "Width and Height of texture must be 1 or greater!");
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		m_MipLevels = UseMipmaps(spec.filtering) ? GetNumMipLevels(size, size) : 1;
		glTextureStorage2D(m_RendererID, m_MipLevels, m_InternalFormat, m_Size, m_Size);

		if(!name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTextureCubemap::~OpenGLTextureCubemap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCubemap::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTextureCubemap::SetData(void* data, int faceIndex, uint32_t size, int level)
	{
		APEX_CORE_ASSERT(size == m_PixelSize * m_Size * m_Size, "Size of data not equal to size of texture!");
		glTextureSubImage3D(m_RendererID, level, 0, 0, faceIndex, m_Size, m_Size, 1, GL_RGB, m_DataType, data);
	}

	void OpenGLTextureCubemap::Invalidate(uint32_t width, uint32_t height)
	{
		APEX_CORE_ERROR("Function (" __FUNCSIG__ ") NOT implemented!");
	}

	void OpenGLTextureCubemap::GenerateMipmap() const
	{
		glGenerateTextureMipmap(m_RendererID);
	}

	//////////////////////////////////////////////////////////////////////
	/*-----------------------Texture 2D Array---------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLTexture2DArray::OpenGLTexture2DArray(uint32_t width, uint32_t height, uint32_t count, const TextureSpec& spec, std::string name)
		: m_Name(std::move(name)), m_Width(width), m_Height(height), m_Count(count),
		  m_InternalFormat(GetOpenGLInternalFormat(spec.internalFormat, spec.dataType)),
		  m_AccessFormat(GetOpenGLAccessFormat(spec.accessFormat)),
		  m_DataType(GetOpenGLDataType(spec.dataType)),
		  m_Filtering(spec.filtering), m_PixelSize(GetOpenGLPixelSize(spec.internalFormat))
	{
		APEX_CORE_ASSERT(width > 0 && height > 0, "Width and Height of texture must be 1 or greater!");
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_RendererID);

		auto [minFilter, magFilter] = GetMinMagFilterPair(m_Filtering);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter);

		if (spec.accessFormat == AF::DEPTH) {
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		m_MipLevels = UseMipmaps(spec.filtering) ? GetNumMipLevels(width, height) : 1;
		glTextureStorage3D(m_RendererID, m_MipLevels, m_InternalFormat, m_Width, m_Height, m_Count);

		if(!m_Name.empty())
			glObjectLabel(GL_TEXTURE, m_RendererID, m_Name.size(), m_Name.data());
	}

	OpenGLTexture2DArray::~OpenGLTexture2DArray()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2DArray::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2DArray::BindImage(uint32_t unit, int index, bool read, bool write, int level) const
	{
		APEX_CORE_ASSERT(read || write, "Image should be atleast readable or writable");
		glBindImageTexture(unit, m_RendererID, level, GL_TRUE, index, (read && write) ? GL_READ_WRITE : (read) ? GL_READ_ONLY : GL_WRITE_ONLY, m_InternalFormat);
	}

	void OpenGLTexture2DArray::Invalidate(uint32_t width, uint32_t height)
	{
		APEX_CORE_ERROR("Function (" __FUNCSIG__ ") NOT implemented!");
	}

	void OpenGLTexture2DArray::SetData(void* data, int index, uint32_t size, int level)
	{
		glTextureSubImage3D(m_RendererID, level, 0, 0, index, m_Width, m_Height, 1, m_AccessFormat, m_DataType, data);
	}

	void OpenGLTexture2DArray::GenerateMipmap() const
	{
		glGenerateTextureMipmap(m_RendererID);
	}

}
