#pragma once
#include "Apex/Layers/Layer.h"

#include <imgui.h>
#include <irrKlang.h>
//#include "EditorTools/NodeGraph/Node.h"
#include "EditorTools/NodeGraph/NodeGraph.h"
#include "EditorTools/PythonGraph/PythonGraph.h"

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
			Apex::Ref<Apex::IndexBuffer> squareIB;
			uint32_t squareIndices[] = {
				0, 1, 2,
				0, 2, 3
			};
			squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
			m_ScreenVA->AddIndexBuffer(squareIB);
			m_ScreenVA->Unbind();

			auto screenShader = AssetManager::GetShaderLibrary().Load("assets/Screen.glsl");
			screenShader->Bind();
			screenShader->SetUniInt("u_ScreenTexture", 0);

			m_Texture = Texture2D::Create("assets/Checkerboard.png");

			m_GameFrameBuffer = FrameBuffer::Create(false);
			m_GameTexture = Texture2D::Create(800, 600);
			m_GameFrameBuffer->AttachTexture(m_GameTexture);

			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
			ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(font_cousine_compressed_data_base85, 14);
			ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/consola.ttf", 14);
		}
		
		virtual void OnDetach() override 
		{
			m_Sound->drop();
			m_SoundEngine->drop();
		}

		virtual void OnUpdate() override 
		{
			RenderCommands::SetClearColor(m_BGColor);
			RenderCommands::Clear();

			m_GameFrameBuffer->Bind();
			RenderCommands::SetClearColor(m_BGColor);
			RenderCommands::Clear();
			
			Renderer::BeginScene(m_Camera);
			auto screenShader = AssetManager::GetShaderLibrary().GetShader("Screen");
			//m_Texture->Bind(0);
			m_GameTexture->Bind(0);
			Renderer::SubmitPostProcess(screenShader, m_ScreenVA);
			Renderer::EndScene();
			m_GameFrameBuffer->Unbind();
		}

		virtual void OnImGuiRender() override
		{
			auto font2 = ImGui::GetIO().Fonts->Fonts[1];
			ImGui::PushFont(font2);
			//ShowDockspace();
			//ImGui::ShowDemoWindow();
			//ImGui::ShowMetricsWindow();

			static EditorTools::NodeGraph nodeGraph(PythonGraph::nodeTypes, PythonGraph::CreateNode, std::vector<std::string>() );
			if (ImGui::Begin("Node Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				nodeGraph.RenderNodeGraph();
			}
			ImGui::End();
			
			ImGui::Begin("Rendering");
			ImGui::DragFloat4("Background", &m_BGColor[0], 0.001f, 0.0f, 1.0f);
			/*if (!m_ShowNodeGraph) {
				ImGui::Button("Show Node Graph");
			}*/
			if (ImGui::Button("Parse Graph"))
				APEX_CORE_TRACE(nodeGraph.Parse().c_str());
			ImGui::End();

			ImGui::PopFont();
			//ShowGameViewport();
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
			ImGui::Image((void*)(intptr_t)m_GameTexture->GetID(), { (float)m_GameTexture->GetWidth(), (float)m_GameTexture->GetHeight() });
			ImGui::End();
		}

	private:
		bool m_Open;
		bool m_ShowNodeGraph = true;

		glm::vec4 m_BGColor;

		OrthographicCamera m_Camera;
		Ref<VertexArray> m_ScreenVA;
		Ref<FrameBuffer> m_GameFrameBuffer;
		Ref<Texture2D> m_GameTexture, m_Texture;

		irrklang::ISoundEngine* m_SoundEngine;
		irrklang::ISound* m_Sound;
	};

}