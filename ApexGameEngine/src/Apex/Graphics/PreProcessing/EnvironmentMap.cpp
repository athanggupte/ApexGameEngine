#include "apex_pch.h"
#include "EnvironmentMap.h"

#include "Apex/Graphics/ShaderDefines.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/RenderPrimitives/Framebuffer.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"

namespace Apex {

	namespace {

		static Ref<Shader> s_EquirectangularToCubemapShader;
		static Ref<Shader> s_DiffuseIrradianceCubemapShader;
		static Ref<Shader> s_PrefilterSpecularCubemapShader;
		static Ref<Shader> s_CreateBRDFLUTShader;

	}


	void EnvironmentMapPreprocess::Init()
	{
		s_EquirectangularToCubemapShader = Shader::Create("internal_assets/shaders/EquirectangularProjection.glsl");
		s_DiffuseIrradianceCubemapShader = Shader::Create("internal_assets/shaders/DiffuseIrradianceCubemap.glsl");
		s_PrefilterSpecularCubemapShader = Shader::Create("internal_assets/shaders/PrefilterSpecularCubemap.glsl");
		s_CreateBRDFLUTShader = Shader::Create("internal_assets/shaders/CreateBRDFLUT.glsl");
	}

	void EnvironmentMapPreprocess::ConvertEquirectangularToCubemap(const Ref<Texture2D>& source_envmap, const Ref<TextureCubemap>& target_cubemap)
	{
		uint32_t width = target_cubemap->GetWidth();
		Ref<Framebuffer> tempFramebuffer = Framebuffer::Create({ width, width, 0, false });

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LEQUAL);

		source_envmap->Bind(1);
		s_EquirectangularToCubemapShader->Bind();
		for (int face_index = 0; face_index < 6; face_index++) {
			tempFramebuffer->AttachColorTextureLayer(target_cubemap, 0, face_index);
			tempFramebuffer->Bind();

			s_EquirectangularToCubemapShader->SetUniInt1("u_FaceIndex", face_index);
			RenderCommands::Clear();
			RenderCommands::Draw(6);
		}

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LESS);
	}

	void EnvironmentMapPreprocess::CreateDiffuseIrradianceMap(const Ref<TextureCubemap>& source_envmap, const Ref<TextureCubemap>& target_cubemap)
	{
		uint32_t width = target_cubemap->GetWidth();
		Ref<Framebuffer> tempFramebuffer = Framebuffer::Create({ width, width, 0, false });

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LEQUAL);
		RenderCommands::Clear(FramebufferTargetBit::DEPTH);

		source_envmap->Bind(TEX_SLOT_ENV_Skybox);
		s_DiffuseIrradianceCubemapShader->Bind();
		for (int face_index = 0; face_index < 6; face_index++) {
			tempFramebuffer->AttachColorTextureLayer(target_cubemap, 0, face_index);
			tempFramebuffer->Bind();

			s_DiffuseIrradianceCubemapShader->SetUniInt1("u_FaceIndex", face_index);
			RenderCommands::Clear();
			RenderCommands::Draw(6);
		}

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LESS);
	}

	void EnvironmentMapPreprocess::CreatePrefilteredSpecularMap(const Ref<TextureCubemap>& source_envmap, const Ref<TextureCubemap>& target_cubemap)
	{
		uint32_t width = target_cubemap->GetWidth();
		Ref<Framebuffer> tempFramebuffer = Framebuffer::Create({ width, width, 0, false });

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LEQUAL);

		source_envmap->Bind(TEX_SLOT_ENV_Skybox);
		s_PrefilterSpecularCubemapShader->Bind();
		//s_PrefilterSpecularCubemapShader->SetUniFloat1("u_MaxMipLevels", s_PrefilteredSpecularCubemapTexture->GetMipLevels());
		const uint32_t maxMipLevels = target_cubemap->GetMipLevels();
		for (int mipLevel = 0; mipLevel < maxMipLevels; mipLevel++) {
			if (mipLevel > 0) {
				width /= 2;
				tempFramebuffer->Resize(width, width);
			}

			const float roughness = static_cast<float>(mipLevel) / static_cast<float>(maxMipLevels - 1);
			s_PrefilterSpecularCubemapShader->SetUniFloat1("u_Roughness", roughness);
			s_PrefilterSpecularCubemapShader->SetUniFloat1("u_ImageWidth", width);
			for (int face_index = 0; face_index < 6; face_index++) {
				tempFramebuffer->AttachColorTextureLayer(target_cubemap, 0, face_index, mipLevel);
				tempFramebuffer->Bind();

				s_PrefilterSpecularCubemapShader->SetUniInt1("u_FaceIndex", face_index);
				RenderCommands::Clear();
				RenderCommands::Draw(6);
			}

		}

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LESS);
	}

	void EnvironmentMapPreprocess::CreateBRDFLUT(const Ref<Texture2D>& target_texture)
	{
		uint32_t width = target_texture->GetWidth();
		Ref<Framebuffer> tempFramebuffer = Framebuffer::Create({ width, width, 0, false });

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LEQUAL);

		tempFramebuffer->AttachColorTexture(target_texture, 0);
		tempFramebuffer->Bind();
		s_CreateBRDFLUTShader->Bind();
		RenderCommands::Clear();
		RenderCommands::Draw(6);

		RenderCommands::SetDepthTestFunction(DepthStencilMode::LESS);
	}

}

