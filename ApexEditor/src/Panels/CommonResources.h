#pragma once

#include <imgui.h>

namespace Apex {

	class Texture;

	struct TextureImageOptions
	{
		ImVec2 size;          // Image Size
		int frame_padding;    // -1 == uses default padding (style.FramePadding)
		ImVec2 uv0;           // UV coordinates for lower-left
		ImVec2 uv1;           // UV coordinates for (32,32) in our texture
		ImVec4 bg_col;        // Black background
		ImVec4 tint_col;      // No tint
	};

	namespace Common
	{
		const Ref<Texture>& GetPlaceholderTexture();
		const TextureImageOptions& GetDefaultImGuiTextureOptions();
	};

}
