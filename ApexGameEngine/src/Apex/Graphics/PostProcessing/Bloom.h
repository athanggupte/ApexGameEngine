#pragma once
#include <memory>

namespace Apex {
	class Texture2D;

	class BloomPass
	{
	public:
		static void Init(uint32_t srcWidth, uint32_t srcHeight);
		static void Render(const Ref<Texture2D>& input_tex, const Ref<Texture2D>& output_tex, float filterRadius, int steps = 8);

		static Ref<Texture2D> _GetSamplingTextures();
		static float& Threshold();
		static float& Knee();
	};
	
}
