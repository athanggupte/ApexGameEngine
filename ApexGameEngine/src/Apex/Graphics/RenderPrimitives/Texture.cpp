#include "apex_pch.h"
#include "Texture.h"

#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Application.h"

#include "Platform/OpenGL/OpenGLTexture.h"

#include <filesystem>

namespace Apex {

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, bool useSRGB, bool useHDR, const TextureFiltering& filtering)
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

	Ref<TextureCubemap> TextureCubemap::Create(const std::array<fs::path, 6>& paths, bool useHDR)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureCubemap>(paths, useHDR);

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

}
