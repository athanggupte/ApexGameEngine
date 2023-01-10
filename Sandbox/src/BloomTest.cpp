#include "pch.h"
#include "BloomTest.h"

#include <imgui.h>

#include "Apex/Graphics/PostProcessing/Bloom.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/ComputeShader/ComputeShader.h"

namespace sandbox
{
	namespace {
		int downsample_steps = 5;
	}

	BloomTest::BloomTest()
		: Layer("Bloom Test")
	{
		m_InputTexture = Apex::Texture2D::Create("assets/textures/thatch_chapel_1k.hdr", false, true);
		m_OutputTexture = Apex::Texture2D::Create("assets/textures/thatch_chapel_1k.hdr", false, true);
		//m_OutputTexture = Apex::Texture2D::Create(m_InputTexture->GetWidth(), m_InputTexture->GetHeight(), Apex::HDRTextureSpec, "OutputTexture");
		Apex::BloomPass::Init(m_InputTexture->GetWidth(), m_InputTexture->GetHeight());
	}

	void BloomTest::OnAttach()
	{
		Apex::BloomPass::Render(m_InputTexture, m_OutputTexture, 0.005f, downsample_steps);
	}

	void BloomTest::OnUpdate(Apex::Timestep)
	{

		Apex::RenderCommands::Clear();
	}

	void BloomTest::OnImGuiRender()
	{
		{
			ImGui::Begin("Textures");
			//if (ImGui::CollapsingHeader("Input Texture"))
				ImGui::Image((void*)(intptr_t)m_InputTexture->GetID(), { 512.f, 512.f });
			ImGui::SameLine();
			//if (ImGui::CollapsingHeader("Output Texture"))
				ImGui::Image((void*)(intptr_t)m_OutputTexture->GetID(), { 512.f, 512.f });

			//ImGui::SliderInt("Steps", &downsample_steps, 1, 10);
			//ImGui::DragFloat("Threshold", &Apex::BloomPass::Threshold(), 0.05f, 0.f, 1.f);
			//ImGui::DragFloat("Knee", &Apex::BloomPass::Knee(), 0.05f, 0.f, 1.f);

			ImGui::End();
		}
	}
}
