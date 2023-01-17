#include "apex_pch.h"
#include "Texture.h"

#include "Apex/Graphics/Renderer/RendererAPI.h"
#include "Apex/Application.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLTextureFactory.h"

namespace Apex {

	Ref<Texture> Texture::Create(TextureDesc desc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return OpenGLTextureFactory::Create(desc);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const fs::path& path, bool useSRGB, bool useHDR, TextureFiltering filtering)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(path, useSRGB, useHDR, filtering);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const TextureSpec& spec, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(width, height, spec, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2DMS> Texture2DMS::Create(uint32_t width, uint32_t height, const TextureSpec& spec, uint32_t samples, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2DMS>(width, height, spec, samples, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<TextureCubemap> TextureCubemap::Create(const std::array<fs::path, 6>& paths, bool useHDR, TextureFiltering filtering)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureCubemap>(paths, useHDR, filtering);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<TextureCubemap> TextureCubemap::Create(uint32_t size, const TextureSpec& spec, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureCubemap>(size, spec, name);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2DArray> Texture2DArray::Create(uint32_t width, uint32_t height, uint32_t count, const TextureSpec& spec, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
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
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D_HDR>(path);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D_HDR> Texture2D_HDR::Create(uint32_t width, uint32_t height, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
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
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureDepth2D>(width, height);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}
#endif

}
