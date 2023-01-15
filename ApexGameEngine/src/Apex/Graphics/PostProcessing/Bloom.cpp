#include "apex_pch.h"
#include "Bloom.h"

#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/ComputeShader/ComputeShader.h"

namespace Apex {

	static uint32_t ceil_div(uint32_t div, uint32_t mod)
	{
		return div / mod + (div % mod ? 1 : 0);
	}

	namespace
	{
		Ref<ComputeShader> s_PrefilterShader;
		Ref<ComputeShader> s_DownsampleShader;
		Ref<ComputeShader> s_UpsampleShader;

		Ref<Texture2D> s_SamplingTexture;


		float threshold = 1.5f;
		float knee = 0.5f;

	}

	void BloomPass::Init(uint32_t srcWidth, uint32_t srcHeight)
	{
		s_PrefilterShader = ComputeShader::Create("internal_assets/shaders/Prefilter.glsl");
		s_DownsampleShader = ComputeShader::Create("internal_assets/shaders/BloomDownsample.glsl");
		s_UpsampleShader = ComputeShader::Create("internal_assets/shaders/BloomUpsample.glsl");

		auto sampling_texture_spec = defaults::HDRTextureSpec;
		sampling_texture_spec.filtering = TextureFiltering::TRILINEAR;
		s_SamplingTexture = Texture2D::Create(srcWidth / 2, srcHeight / 2, sampling_texture_spec, "Bloom: Sampling Texture");
	}

	void BloomPass::Render(const Ref<Texture2D>& input_tex, const Ref<Texture2D>& output_tex, float filterRadius, int steps)
	{
		uint32_t width = input_tex->GetWidth(), height = input_tex->GetHeight();

		input_tex->Bind(0);

		int steps_completed = 0;

		s_DownsampleShader->Bind();
		s_DownsampleShader->SetUniFloat4("u_PrefilterThreshold", glm::vec4{ threshold, threshold - knee, knee * 2, 0.25 / knee });

		std::vector<std::pair<uint32_t, uint32_t>> resolutions;

		for (; steps_completed < steps; steps_completed++) {
			width /= 2;
			height /= 2;

			resolutions.push_back({width, height});

			if (width < 8 || height < 8) {
				steps_completed++;
				break;
			}

			// Bind output texture
			s_SamplingTexture->BindImage(1, false, true, steps_completed);

			// Bind and dispatch compute shader invocations
			s_DownsampleShader->Bind();
			s_DownsampleShader->SetUniFloat2("srcResolution", glm::vec2{ width * 2, height * 2 });
			s_DownsampleShader->SetUniFloat2("dstResolution", glm::vec2{ width, height });
			s_DownsampleShader->SetUniFloat1("inLod", static_cast<float>(steps_completed - 1));
			s_DownsampleShader->Dispatch(ceil_div(width, 8), ceil_div(height, 8), 1);

			// Call image access barrier to ensure that the write operations on the textures are completed before moving forward
			RenderCommands::ImageAccessBarrier();

			// Bind source texture for second iteration
			if (steps_completed == 0)
				s_SamplingTexture->Bind(0);
		}

		steps_completed--;

		s_UpsampleShader->Bind();
		s_UpsampleShader->SetUniInt1("isFinal", 0);
		s_UpsampleShader->SetUniFloat1("filterRadius", filterRadius);
		for (; steps_completed > 0; steps_completed--) {
			//width *= 2;
			//height *= 2;
			auto [w, h] = resolutions[steps_completed - 1];


			// Bind output texture
			s_SamplingTexture->BindImage(1, true, true, steps_completed - 1);

			// Bind source texture
			//s_SamplingTexture->Bind(0);
			s_UpsampleShader->SetUniFloat1("inLod", static_cast<float>(steps_completed));
			s_UpsampleShader->SetUniFloat2("srcResolution", glm::vec2{ w / 2, h / 2 });
			s_UpsampleShader->SetUniFloat2("dstResolution", glm::vec2{ w, h });

			// Bind and dispatch compute shader invocations
			s_UpsampleShader->Bind();
			s_UpsampleShader->Dispatch(ceil_div(w, 8), ceil_div(h, 8), 1);

			// Call image access barrier to ensure that the write operations on the textures are completed before moving forward
			RenderCommands::ImageAccessBarrier();
		}

		// Bind output texture
		output_tex->BindImage(1U, false, true);

		auto [w, h] = resolutions[steps_completed];

		// Bind and dispatch compute shader invocations
		s_UpsampleShader->Bind();
		s_UpsampleShader->SetUniInt1("isFinal", 1);
		s_UpsampleShader->SetUniFloat1("inLod", static_cast<float>(steps_completed));
		s_UpsampleShader->SetUniFloat2("srcResolution", glm::vec2{ w, h });
		s_UpsampleShader->SetUniFloat2("dstResolution", glm::vec2{ output_tex->GetWidth(), output_tex->GetHeight() });
		s_UpsampleShader->Dispatch(ceil_div(output_tex->GetWidth(), 8), ceil_div(output_tex->GetHeight(), 8), 1);

		// Call image access barrier to ensure that the write operations on the textures are completed before moving forward
		RenderCommands::ImageAccessBarrier();
	}

	Ref<Texture2D> BloomPass::_GetSamplingTextures()
	{
		return s_SamplingTexture;
	}

	float& BloomPass::Threshold()
	{
		return threshold;
	}

	float& BloomPass::Knee()
	{
		return knee;
	}
}
