#include "apexed_pch.h"
#include "CommonResources.h"

#include "Apex/Graphics/RenderPrimitives/Texture.h"

#include <imgui.h>

namespace Apex {

	namespace internal {
		static Ref<Texture> s_PlaceholderTexture;
		static TextureImageOptions s_ImageOptions {
			{ 50.0f, 50.0f },
			-1,
			{ 0.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f }
		};
	}

	const Ref<Texture>& Common::GetPlaceholderTexture()
	{
		if (!internal::s_PlaceholderTexture) {
			internal::s_PlaceholderTexture = Texture2D::Create(2, 2, defaults::SimpleTextureSpec, "PlaceholderTexture");
			uint32_t texData[4] = { 0xaaaaaaaa, 0xdddddddd, 0xdddddddd, 0xaaaaaaaa };
			internal::s_PlaceholderTexture->SetData(texData, sizeof(texData));
		}
		return internal::s_PlaceholderTexture;
	}

	const TextureImageOptions& Common::GetDefaultImGuiTextureOptions()
	{
		return internal::s_ImageOptions;
	}
}
