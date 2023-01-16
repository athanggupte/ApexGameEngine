#include "apex_pch.h"
#include "OpenGLTextureFactory.h"

#include "OpenGLTexture.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {

	bool OpenGLTextureFactory::Validate(const TextureDesc& desc)
	{
		if (desc.accessFormat == TextureAccessFormat::DEPTH &&
			(desc.internalFormat < TextureInternalFormat::DEPTH16 ||
			desc.internalFormat > TextureInternalFormat::D32S8))
			return false;

		if (desc.accessFormat == TextureAccessFormat::STENCIL && desc.internalFormat != TextureInternalFormat::STENCIL8)
			return false;


		return true;
	}

	Ref<Texture> OpenGLTextureFactory::Create(const TextureDesc& desc)
	{
		if (!Validate(desc)) {
			APEX_CORE_CRITICAL("Invalid TextureDesc object! Please provide valid TextureDesc.");
			return {};
		}

		switch (desc.type)
		{
		case TextureType::Texture2D: return CreateRef<OpenGLTexture2D>(desc);
		case TextureType::TextureCubemap: break;
		case TextureType::Texture2DMS: break;
		case TextureType::Texture3D: break;
		case TextureType::Texture2DArray: break;
		case TextureType::TextureCubemapArray: break;
		default: ;
		}
	}

}
