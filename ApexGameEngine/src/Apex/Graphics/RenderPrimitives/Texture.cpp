#include "apex_pch.h"
#include "Texture.h"

#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Application.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Apex {

	Ref<Texture2D> Texture2D::Create(const fs::path& path, bool useSRGB, bool useHDR, TextureFiltering filtering)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(path, useSRGB, useHDR, filtering);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const TextureSpec& spec, const std::string& name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(width, height, spec, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2DMS> Texture2DMS::Create(uint32_t width, uint32_t height, const TextureSpec& spec, uint32_t samples, const std::string& name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2DMS>(width, height, spec, samples, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<TextureCubemap> TextureCubemap::Create(const std::array<fs::path, 6>& paths, bool useHDR, TextureFiltering filtering)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureCubemap>(paths, useHDR, filtering);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<TextureCubemap> TextureCubemap::Create(uint32_t size, const TextureSpec& spec, const std::string& name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureCubemap>(size, spec, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2DArray> Texture2DArray::Create(uint32_t width, uint32_t height, uint32_t count, const TextureSpec& spec, const std::string& name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2DArray>(width, height, count, spec, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

#ifdef SEPARATE_HDR
	Ref<Texture2D_HDR> Texture2D_HDR::Create(const std::string & path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D_HDR>(path);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D_HDR> Texture2D_HDR::Create(uint32_t width, uint32_t height, const std::string& name)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D_HDR>(width, height, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}
#endif

#ifdef SEPARATE_DEPTH_CLASS
  	Ref<TextureDepth2D> TextureDepth2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureDepth2D>(width, height);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}
#endif


	//////////// Resource Factory for Texture ////////////

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetSource(const fs::path& file)
	{
		m_SourceFile = file;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetData(void* data, uint32_t size)
	{
		m_Data = data;
		m_DataSize = size;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::CopyFrom(const Ref<Texture>& ptr)
	{
		m_Width = ptr->GetWidth();
		m_Height = ptr->GetHeight();

		m_SpecificationSet = false;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetTextureType(TextureType type)
	{
		m_Type = type;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetSRGB(bool srgb)
	{
		m_sRGB = srgb;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetHDR(bool hdr)
	{
		m_HDR = hdr;
		return *this;
	}

	/*ResourceFactory<Texture>& ResourceFactory<Texture>::SetDataType(TextureDataType data_type)
	{
		m_Specification.dataType = data_type;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetAccessFormat(TextureAccessFormat access_format)
	{
		m_Specification.accessFormat = access_format;
		return *this;
	}*/

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetFiltering(TextureFiltering filtering)
	{
		m_Specification.filtering = filtering;
		return *this;
	}

	ResourceFactory<Texture>& ResourceFactory<Texture>::SetSpecification(TextureSpec spec)
	{
		m_Specification = spec;
		return *this;
	}

	Ref<Texture> ResourceFactory<Texture>::Create(const std::string& name) const
	{
		APEX_CORE_ASSERT(m_Data || !m_SourceFile.empty(), "Cannot build texture without source!");
		switch (m_Type)
		{
		case TextureType::Texture2D:
			{
				if (!m_SourceFile.empty())
					return Texture2D::Create(m_SourceFile, m_sRGB, m_HDR, m_Specification.filtering);
				auto tex = Texture2D::Create(m_Width, m_Height, m_Specification, name);
				tex->SetData(m_Data, m_DataSize);
				return tex;
			}
		case TextureType::TextureCubemap:
			{
				
				break;
			}
#ifdef SEPARATE_DEPTH_CLASS
  		case TextureType::TextureDepth2D:
			{
				
				break;
			}
#endif

		case TextureType::Texture2DMS:
			{
				
				break;
			}
		}
	}
}
