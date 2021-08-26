#pragma once
#include "Apex/Core/Layers/Layer.h"

#include <imgui.h>
//#include <irrKlang.h>
//#include "EditorTools/NodeGraph/Node.h"
//#include "EditorTools/NodeGraph/NodeGraph.h"
//#include "EditorTools/PythonGraph/PythonGraph.h"
// #include "EditorTools/ShaderGraph/ShaderGraph.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef APEX_INSTALL_LOCATION
#error Install location not specified
#endif

extern const char font_cousine_compressed_data_base85[];

namespace Apex {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: Layer("ApexEditor"), m_BGColor{0.42f, 0.63f, 0.75f, 1.0f},
			m_CameraController((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight())
		{
			AssetManager::Init();
		}

		virtual void OnAttach() override
		{
			// Mount required directories
			FileSystem::Mount("/assets", APEX_INSTALL_LOCATION "/assets");

			// Asset allocation
			m_ImageTexture = Texture2D::Create(256U, 256U, HDRTextureSpec, "Image");
			m_ComputeShader = ComputeShader::Create("/assets/Blur.compute");
			m_Texture = Texture2D::Create("/assets/pusheen-thug-life.png");
			m_GameFramebuffer = Framebuffer::Create({ 800U, 600U });

			// Entity Initialization
			auto pusheenEntity = m_Scene.CreateEntity("pusheen");
			pusheenEntity.AddComponent<SpriteRendererComponent>(m_Texture, 2.f);
			pusheenEntity.GetComponents<TransformComponent>().Transform = glm::translate(glm::mat4(1.f), { 0.6f, 0.1f, 0.f }) * glm::scale(glm::mat4(1.f), { 0.5f, 0.5f, 1.f });
			
			auto squareEntity = m_Scene.CreateEntity("square");
			squareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.2f, 0.88f, 0.34f, 1.f });
			squareEntity.GetComponents<TransformComponent>().Transform = glm::translate(glm::mat4(1.f), { -0.6f, -0.1f, 0.f }) * glm::scale(glm::mat4(1.f), { 0.5f, 0.5f, 1.f });
			
			auto cameraEntity = m_Scene.CreateEntity("camera");
			auto aspectRatio = 16.f / 9.f;
			cameraEntity.AddComponent<CameraComponent>(glm::ortho(aspectRatio * -1.f, aspectRatio * 1.f, -1.f, 1.f, -1.f, 1.f));
			m_Scene.SetPrimaryCamera(cameraEntity);
			
			// ImGui options
			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
			ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(font_cousine_compressed_data_base85, 12);
			ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/consola.ttf", 12);
		}
		
		virtual void OnDetach() override 
		{
			AssetManager::Shutdown();
			//m_Sound->drop();
			//m_SoundEngine->drop();
		}

		virtual void OnUpdate(Timestep ts) override 
		{
			// Resize
			if (auto& fbSpec = m_GameFramebuffer->GetSpecification();
				m_GameViewportSize.x > 0.f && m_GameViewportSize.y > 0.f &&
				(fbSpec.width != m_GameViewportSize.x || fbSpec.height != m_GameViewportSize.y))
			{
				//m_GameFramebuffer->Resize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
				m_CameraController.OnResize(m_GameViewportSize.x, m_GameViewportSize.y);
			}
			
			m_CameraController.OnUpdate();
			
			
			// Render
			Renderer2D::ResetStats();
			
			m_GameFramebuffer->Bind();
			RenderCommands::SetClearColor(m_BGColor);
			RenderCommands::Clear();
			
			m_Scene.OnUpdate(m_PlayScene);
			if (!m_PlayScene) {
				Renderer2D::BeginScene(m_CameraController.GetCamera());
				m_Scene.Render2D();
				Renderer2D::EndScene();
			}
			
			
			m_GameFramebuffer->Unbind();
		}

		virtual void OnEvent(Event& e) override
		{
			m_CameraController.OnEvent(e);
		}
		
		virtual void OnImGuiRender() override
		{
			auto font2 = ImGui::GetIO().Fonts->Fonts[1];
			ImGui::PushFont(font2);
			BeginDockspace();
			
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
			
			{
				ImGui::Separator();
				ImGui::Checkbox("Play", &m_PlayScene);
				ImGui::Separator();
			}
			
			{
				ImGui::Separator();
				ImGui::Text("Entities:");
				ImGui::Separator();
			}
			
			//if (!m_ShowNodeGraph) {
			//	m_ShowNodeGraph = ImGui::Button("Show Node Graph");
			//}

			//if (ImGui::Button("Parse Graph"))
			//	APEX_CORE_TRACE(nodeGraph.Parse().c_str());
			ImGui::End();

			ShowSettings();
			ShowGameViewport();
			EndDockspace();
			
			ImGui::PopFont();
		}

		void ShowSettings()
		{
			ImGui::Begin("Settings");
			
			auto stats = Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Triangles: %d", stats.GetTriangleCount());
			ImGui::Text("Vertices: %d", stats.GetVertexCount());
			ImGui::Text("Indices: %d", stats.GetIndexCount());
			
			ImGui::End();
		}
		
		void ShowGameViewport()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
			ImGui::Begin("Game View");
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_GameViewportSize = *((glm::vec2*)&viewportSize);
			
			ImGui::Image((void*)(intptr_t)m_GameFramebuffer->GetColorAttachmentID(), { m_GameViewportSize.x, m_GameViewportSize.y }, { 0.f, 1.f }, { 1.f, 0.f });
			ImGui::End();
			ImGui::PopStyleVar();
		}
		
		void BeginDockspace()
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
		}
		
		void EndDockspace()
		{
			// Clear out Dockspace vars
			ImGui::End();
		}

		void ComputeShaderOutput()
		{
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
				m_ComputeShader->SetUniFloat2("u_BlurAmount", glm::vec2{ 8.f, 8.f });
				
				m_ComputeShader->Dispatch(m_ImageTexture->GetWidth(), m_ImageTexture->GetHeight(), 1U);
			}
			if (ImGui::Button("Run-Blur-Screen")) {
				m_ComputeShader->Bind();
				m_ImageTexture->BindImage(0, false, true);

				m_GameFramebuffer->GetColorAttachment()->Bind(0);
				m_ComputeShader->SetUniFloat2("u_BlurAmount", glm::vec2{ 8.f, 8.f });

				m_ComputeShader->Dispatch(m_ImageTexture->GetWidth(), m_ImageTexture->GetHeight(), 1U);
			}
			ImGui::Image((void*)(intptr_t)m_ImageTexture->GetID(), { (float)m_ImageTexture->GetWidth(), (float)m_ImageTexture->GetHeight() },
				{ 0, 0 }, { 1, 1 }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f });
		}
		
	private:
		bool m_Open;
		bool m_ShowNodeGraph = true;

		glm::vec4 m_BGColor;

		Ref<Framebuffer> m_GameFramebuffer;
		glm::vec2 m_GameViewportSize;
		
		OrthographicCameraController2D m_CameraController;
		Scene m_Scene;
		
		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_ImageTexture;		
		Ref<ComputeShader> m_ComputeShader;

		bool m_PlayScene = false;
		
		//irrklang::ISoundEngine* m_SoundEngine;
		//irrklang::ISound* m_Sound;
	};

}
