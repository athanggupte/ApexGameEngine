#pragma once
#include "Apex/Core/Layers/Layer.h"

#include <imgui.h>
#include <irrKlang.h>
//#include "EditorTools/NodeGraph/Node.h"
//#include "EditorTools/NodeGraph/NodeGraph.h"
//#include "EditorTools/PythonGraph/PythonGraph.h"
#include "EditorTools/ShaderGraph/ShaderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern const char font_cousine_compressed_data_base85[];

namespace Apex {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: m_BGColor{0.42f, 0.63f, 0.75f, 1.0f}, m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
		{
		}

		virtual void OnAttach() override
		{
			{
				m_ScreenVA = Apex::VertexArray::Create();
				Apex::Ref<Apex::VertexBuffer> screenVB;
				float screenVertices[] = {
					-1.0f,  1.0f,  0.0f, 1.0f,
					-1.0f, -1.0f,  0.0f, 0.0f,
					 1.0f, -1.0f,  1.0f, 0.0f,

					-1.0f,  1.0f,  0.0f, 1.0f,
					 1.0f, -1.0f,  1.0f, 0.0f,
					 1.0f,  1.0f,  1.0f, 1.0f
				};
				screenVB = Apex::VertexBuffer::Create(screenVertices, sizeof(screenVertices));
				screenVB->SetLayout({
					{ Apex::ShaderDataType::Float2, "a_Position" },
					{ Apex::ShaderDataType::Float2, "a_TexCoord" }
					});
				m_ScreenVA->AddVertexBuffer(screenVB);
				m_ScreenVA->Unbind();
			}

			{
				m_TextureVA = Apex::VertexArray::Create();
				Apex::Ref<Apex::VertexBuffer> textureVB;
				float textureVertices[] = {
					-1.0f,  1.0f, -0.1f,   0.0f, 1.0f,
					-1.0f, -1.0f, -0.1f,   0.0f, 0.0f,
					 1.0f, -1.0f, -0.1f,   1.0f, 0.0f,
					 1.0f,  1.0f, -0.1f,   1.0f, 1.0f
				};
				textureVB = Apex::VertexBuffer::Create(textureVertices, sizeof(textureVertices));
				textureVB->SetLayout({
					{ Apex::ShaderDataType::Float3, "a_Position" },
					{ Apex::ShaderDataType::Float2, "a_TexCoord" }
					});
				m_TextureVA->AddVertexBuffer(textureVB);
				Apex::Ref<Apex::IndexBuffer> squareIB;
				uint32_t squareIndices[] = {
					0, 1, 2,
					2, 3, 0
				};
				squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
				m_TextureVA->AddIndexBuffer(squareIB);
				m_TextureVA->Unbind();
			}
			auto screenShader = AssetManager::GetShaderLibrary().Load("assets/Screen.glsl");
			screenShader->Bind();
			screenShader->SetUniInt1("u_ScreenTexture", 0);

			auto textureShader = AssetManager::GetShaderLibrary().Load("assets/SimpleTexture.glsl");
			textureShader->Bind();
			textureShader->SetUniInt1("u_Texture", 1);

			m_ImageTexture = Texture2D_HDR::Create(128U, 128U, "Image");
			m_ComputeShader = ComputeShader::Create("assets/Blur.compute");

			m_Texture = Texture2D::Create("assets/pusheen-thug-life.png");

			m_GameFrameBuffer = FrameBuffer::Create(false);
			m_GameTexture = Texture2D::Create(800U, 600U);
			m_GameFrameBuffer->AttachTexture(m_GameTexture);

			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
			ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(font_cousine_compressed_data_base85, 14);
			ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/consola.ttf", 14);
		}
		
		virtual void OnDetach() override 
		{
			//m_Sound->drop();
			//m_SoundEngine->drop();
		}

		virtual void OnUpdate() override 
		{
			//RenderCommands::SetClearColor(m_BGColor);
			//RenderCommands::Clear();

			m_GameFrameBuffer->Bind();
			RenderCommands::SetViewport(0, 0, 800, 600);
			RenderCommands::SetClearColor(m_BGColor);
			RenderCommands::Clear();
			
			Renderer::BeginScene(m_Camera);

			auto textureShader = AssetManager::GetShaderLibrary().GetShader("SimpleTexture");
			
			m_Texture->Bind(1);

			Renderer::Submit(textureShader, m_TextureVA, glm::scale(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 1.0f)));
			Renderer::EndScene();
			m_GameFrameBuffer->Unbind();

			//auto screenShader = AssetManager::GetShaderLibrary().GetShader("Screen");
			//m_GameTexture->Bind(0);
			//Renderer::SubmitPostProcess(screenShader, m_ScreenVA);
		}

		virtual void OnImGuiRender() override
		{
			auto font2 = ImGui::GetIO().Fonts->Fonts[1];
			ImGui::PushFont(font2);
			ShowDockspace();
			//ImGui::ShowDemoWindow();
			//ImGui::ShowMetricsWindow();

			/*static EditorTools::NodeGraph nodeGraph(PythonGraph::nodeTypes, PythonGraph::CreateNode, std::vector<std::string>() );
			if (ImGui::Begin("Node Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				nodeGraph.RenderNodeGraph();
			}
			ImGui::End();*/
			/*
			static EditorTools::ShaderGraph shaderGraph;
			if (m_ShowNodeGraph) {
				if (ImGui::Begin("Shader Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
					shaderGraph.RenderShaderGraph();
				}
				ImGui::End();
			}*/

			ImGui::Begin("Rendering");
			ImGui::DragFloat4("Background", &m_BGColor[0], 0.001f, 0.0f, 1.0f);
			//if (!m_ShowNodeGraph) {
			//	m_ShowNodeGraph = ImGui::Button("Show Node Graph");
			//}
			if (ImGui::Button("Run-Basic-Texture")) {
				m_ComputeShader->Bind();
				m_ImageTexture->BindImage(0, false, true);

				m_Texture->Bind(0);
				m_ComputeShader->SetUniFloat2("u_BlurAmount", glm::vec2{ 0.f, 0.f });

				m_ComputeShader->Dispatch(m_ImageTexture->GetWidth(), m_ImageTexture->GetHeight(), 1U);
			}
			if (ImGui::Button("Run-Blur-Texture")) {
				m_ComputeShader->Bind();
				m_ImageTexture->BindImage(0, false, true);
				
				m_Texture->Bind(0);				
				m_ComputeShader->SetUniFloat2("u_BlurAmount", glm::vec2{ 4.f, 4.f });
				
				m_ComputeShader->Dispatch(m_ImageTexture->GetWidth(), m_ImageTexture->GetHeight(), 1U);
			}
			if (ImGui::Button("Run-Blur-Screen")) {
				m_ComputeShader->Bind();
				m_ImageTexture->BindImage(0, false, true);

				m_GameTexture->Bind(0);
				m_ComputeShader->SetUniFloat2("u_BlurAmount", glm::vec2{ 4.f, 4.f });

				m_ComputeShader->Dispatch(m_ImageTexture->GetWidth(), m_ImageTexture->GetHeight(), 1U);
			}
			ImGui::Image((void*)(intptr_t)m_ImageTexture->GetID(), { (float)m_ImageTexture->GetWidth(), (float)m_ImageTexture->GetHeight() },
				{ 0, 0 }, { 1, 1 }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f });

			//if (ImGui::Button("Parse Graph"))
			//	APEX_CORE_TRACE(nodeGraph.Parse().c_str());
			ImGui::End();

			ShowGameViewport();
			ImGui::PopFont();
		}

		void ShowDockspace()
		{
			ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None;
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			windowFlags |= ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoBringToFrontOnFocus
				| ImGuiWindowFlags_NoNavFocus;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
			ImGui::Begin("Dockspace", &m_Open, windowFlags);
			ImGui::PopStyleVar(3); // Pop StyleVars - WindowPadding, WindowBorderSize, WindowRounding

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			ImGuiID dockspaceID = ImGui::GetID("EditorDockspace");
			ImGui::DockSpace(dockspaceID, { 0.0f, 0.0f }, dockNodeFlags);

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit")) Application::Get().Close();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		void ShowGameViewport()
		{
			ImGui::Begin("Game View");
			auto window_size = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
			static ImVec2 windowSize { 0.f, 0.f };
			static ImVec2 viewportSize { 0.f, 0.f };
			if (windowSize.x != window_size.x || windowSize.y != window_size.y) {
				windowSize = window_size;
				ImVec2 ratio = { windowSize.x / (float)m_GameTexture->GetWidth(), windowSize.y / (float)m_GameTexture->GetHeight() };
				viewportSize = (ratio.x < ratio.y) ? ImVec2(windowSize.x, windowSize.y * ratio.x) : ImVec2(windowSize.x * ratio.y, windowSize.y);
			}
			ImGui::Image((void*)(intptr_t)m_GameTexture->GetID(), viewportSize);
			ImGui::End();
		}

	private:
		bool m_Open;
		bool m_ShowNodeGraph = true;

		glm::vec4 m_BGColor;

		OrthographicCamera m_Camera;
		Ref<VertexArray> m_ScreenVA;
		Ref<VertexArray> m_TextureVA;
		Ref<FrameBuffer> m_GameFrameBuffer;
		Ref<Texture2D> m_GameTexture, m_Texture;
		Ref<Texture2D_HDR> m_ImageTexture;
		
		Ref<ComputeShader> m_ComputeShader;

		irrklang::ISoundEngine* m_SoundEngine;
		irrklang::ISound* m_Sound;
	};

}
