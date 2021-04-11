#include "apex_pch.h"
#include "Blur.h"

#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "PostProcess.h"

namespace Apex {

	Blur::Blur()
	{
		m_BlurShader = Shader::Create("assets/shaders/GaussianBlur.glsl");
		m_VerBlurTex = Texture2D::Create(1280, 720, { TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT });
		m_HorBlurTex = Texture2D::Create(1280, 720, { TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT });
		m_VerBlurFBO = Framebuffer::Create({ 1280, 720, false, 0 });
		m_HorBlurFBO = Framebuffer::Create({ 1280, 720, false, 0 });
	}

	Ref<Texture> Blur::GaussianBlur(int amount, const Ref<Texture>& inputTexture)
	{

		m_BlurShader->Bind();
		m_BlurShader->SetUniInt1("u_ScreenTexture", 0);
		m_BlurShader->SetUniFloat2("u_Resolution", glm::vec2(1280.f, 720.f));

		//Ref<Shader> m_BlurShader = Shader::Create("assets/shaders/GaussianBlur.glsl");
		//Ref<Texture2D_HDR> m_VerBlurTex = Texture2D_HDR::Create();
		//Ref<Texture2D_HDR> m_HorBlurTex = Texture2D_HDR::Create();
		//Ref<Framebuffer> m_VerBlurFBO = Framebuffer::Create(false);
		//Ref<Framebuffer> m_HorBlurFBO = Framebuffer::Create(false);

		m_VerBlurFBO->AttachColorTexture(m_VerBlurTex);
		m_HorBlurFBO->AttachColorTexture(m_HorBlurTex);

		m_VerBlurFBO->Bind();
		Apex::RenderCommands::SetDepthTest(false);
		//Apex::RenderCommands::SetClearColor({ 0.0f, 0.0f, 1.0f, 1.0f });
		Apex::RenderCommands::Clear();
		m_BlurShader->Bind();
		inputTexture->Bind();
		m_BlurShader->SetUniInt1("u_Horizontal", 0);
		Apex::PostProcess::Submit(m_BlurShader);

		for (int i = 0; i < amount; i++) {
			m_HorBlurFBO->Bind();
			Apex::RenderCommands::SetDepthTest(false);
			//Apex::RenderCommands::SetClearColor({ 0.0f, 1.0f, 0.0f, 1.0f });
			Apex::RenderCommands::Clear();
			m_VerBlurTex->Bind();
			m_BlurShader->Bind();
			m_BlurShader->SetUniInt1("u_Horizontal", 1);
			Apex::PostProcess::Submit(m_BlurShader);

			m_VerBlurFBO->Bind();
			Apex::RenderCommands::SetDepthTest(false);
			Apex::RenderCommands::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
			Apex::RenderCommands::Clear();
			m_HorBlurTex->Bind();
			m_BlurShader->Bind();
			m_BlurShader->SetUniInt1("u_Horizontal", 0);
			Apex::PostProcess::Submit(m_BlurShader);
		}

		m_HorBlurFBO->Bind();
		Apex::RenderCommands::SetDepthTest(false);
		//Apex::RenderCommands::SetClearColor({ 0.0f, 1.0f, 0.0f, 1.0f });
		Apex::RenderCommands::Clear();
		m_VerBlurTex->Bind();
		m_BlurShader->Bind();
		m_BlurShader->SetUniInt1("u_Horizontal", 1);
		Apex::PostProcess::Submit(m_BlurShader);

		return m_HorBlurTex;
	}


}
