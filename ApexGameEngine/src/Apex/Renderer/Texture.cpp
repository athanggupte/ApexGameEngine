#include "apex_pch.h"
#include "Texture.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Apex {

	Ref<Texture2D> Texture2D::Create(const std::string & path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture2D>(path);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture2D>();

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D_HDR> Texture2D_HDR::Create(const std::string & path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture2D_HDR>(path);

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<Texture2D_HDR> Texture2D_HDR::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTexture2D_HDR>();

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}

	Ref<TextureDepth2D> TextureDepth2D::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	APEX_CORE_CRITICAL("No Rendering API selected"); return nullptr;
		case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLTextureDepth2D>();

		default:				APEX_CORE_CRITICAL("Unknown Rendering API"); return nullptr;
		}
		return nullptr;
	}
}