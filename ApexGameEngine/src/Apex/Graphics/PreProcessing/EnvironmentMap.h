#pragma once

namespace Apex {
	class Texture2D;
	class TextureCubemap;

	class EnvironmentMapPreprocess
	{
	public:
		static void Init();

		static void ConvertEquirectangularToCubemap(const Ref<Texture2D>& source_envmap, const Ref<TextureCubemap>& target_cubemap);
		static void CreateDiffuseIrradianceMap(const Ref<TextureCubemap>& source_envmap, const Ref<TextureCubemap>& target_cubemap);
		static void CreatePrefilteredSpecularMap(const Ref<TextureCubemap>& source_envmap, const Ref<TextureCubemap>& target_cubemap);
		static void CreateBRDFLUT(const Ref<Texture2D>& target_texture);

	};
}
