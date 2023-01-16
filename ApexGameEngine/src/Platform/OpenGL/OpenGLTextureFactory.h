#pragma once
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {

	class OpenGLTextureFactory
	{
	public:
		static bool Validate(const TextureDesc& desc);
		static Ref<Texture> Create(const TextureDesc& desc);
	};

}
