#include <apexed_pch.h>
#include <Apex.h>

#include "EditorLayer.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Utils/CustomDataStructures.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Core/ECS/SceneSerializer.h"
#include "Apex/Core/ResourceManager/ResourceSerializer.h"

// #include "EditorTools/NodeGraph/Node.h"
// #include "EditorTools/NodeGraph/NodeGraph.h"
 //#include "EditorTools/PythonGraph/PythonGraph.h"
 //#include "EditorTools/ShaderGraph/ShaderGraph.h"


#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Apex {
	
	EditorLayer::EditorLayer()
		: Layer("ApexEditor"), m_BGColor{0.42f, 0.63f, 0.75f, 1.0f},
		m_CameraController((float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight())
	{
		// Logger
		m_LogSink = std::make_shared<EditorLogSink_mt>(&m_LogPanel);
		// Log::GetCoreLogger()->sinks().push_back(m_LogSink);
		Log::GetClientLogger()->sinks().push_back(m_LogSink);
	}

	void EditorLayer::OnAttach()
	{
		FileSystem::Mount("/assets", APEX_INSTALL_LOCATION "/assets");

		m_Scene = CreateRef<Scene>();

		auto& pusheenResource = Application::Get().GetResourceManager().AddResource<Texture>(HASH("pusheen-texture"), HASH("/assets/pusheen-thug-life.png"));
		//Application::Get().GetResourceManager().AddResource(HASH("pusheen-texture"), Resource(HASH("/assets/pusheen-thug-life.png")));
		//APEX_LOG_DEBUG("pusheen-texture :: type: {}", typeid(Application::Get().GetResourceManager().GetResource<Texture>(HASH("pusheen-texture"))).name());
		Ref<ResourceSerializer> rs = ResourceSerializerFactory().SetFormat(ResourceSerializerFactory::Format::XML).Build(Application::Get().GetResourceManager());
		rs->SerializeResource(FileSystem::MakeFile("/assets/pusheen-texture.xml"), pusheenResource);
		pusheenResource.Load();

		// Asset allocation
		m_ImageTexture = Texture2D::Create(256U, 256U, HDRTextureSpec, "Image");
		m_ComputeShader = ComputeShader::Create("/assets/Blur.compute");
		// m_Texture = Texture2D::Create("/assets/pusheen-thug-life.png");
		m_GameFramebuffer = Framebuffer::Create({ 1280u, 720u });

		// Entity Initialization
		//auto pusheenEntity = m_Scene->CreateEntity(HASH("pusheen"));
		//pusheenEntity.AddComponent<SpriteRendererComponent>(pusheenResource, 2.f);
		//pusheenEntity.GetComponent<TransformComponent>() = TransformComponent({ 0.6f, 0.1f, 0.f }, { 0.5f, 0.5f, 1.f });
		//
		//auto squareEntity = m_Scene->CreateEntity(HASH("square"));
		//squareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.2f, 0.88f, 0.34f, 1.f });
		//squareEntity.GetComponents<TransformComponent>() = TransformComponent({ -0.6f, -0.1f, 0.f }, { 0.5f, 0.5f, 1.f });
		//
		//auto cameraEntity = m_Scene->CreateEntity(HASH("camera"));
		//cameraEntity.AddComponent<CameraComponent>(SceneCamera::ProjectionType::Orthographic);
		//m_Scene->SetPrimaryCamera(cameraEntity);
		
		m_Scene->OnSetup();

		// Panels
		m_SceneHeirarchyPanel.SetContext(m_Scene);

		// ImGui options
		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
		ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(font_cousine_compressed_data_base85, 12);
		ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/consola.ttf", 12);
	}
	
	void EditorLayer::OnDetach() 
	{
		//m_Sound->drop();
		//m_SoundEngine->drop();
	}

	void EditorLayer::OnUpdate(Timestep ts) 
	{
		// Resize
		if (auto& fbSpec = m_GameFramebuffer->GetSpecification();
			m_GameViewportSize.x > 0.f && m_GameViewportSize.y > 0.f &&
			(fbSpec.width != m_GameViewportSize.x || fbSpec.height != m_GameViewportSize.y))
		{
			//m_GameFramebuffer->Resize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
			m_CameraController.OnResize(m_GameViewportSize.x, m_GameViewportSize.y);
			m_Scene->OnViewportResize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
		}
		
		if (m_ViewportFocussed)
			m_CameraController.OnUpdate(ts);
		
		
		// Render
		Renderer2D::ResetStats();
		
		m_GameFramebuffer->Bind();
		RenderCommands::SetClearColor(m_BGColor);
		RenderCommands::Clear();
		
		
		if (!m_PlayScene) {
			Renderer2D::BeginScene(m_CameraController.GetCamera());
			m_Scene->DrawSprites();
			Renderer2D::EndScene();
		} else {
			m_Scene->OnUpdate(ts);
		}
		
		m_GameFramebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
	
	static bool show_imgui_demo_window = false;
	
	void EditorLayer::OnImGuiRender()
	{
		auto font2 = ImGui::GetIO().Fonts->Fonts[1];
		ImGui::PushFont(font2);
		BeginDockspace();
		ShowMainMenuBar();
		
		if (show_imgui_demo_window) ImGui::ShowDemoWindow(&show_imgui_demo_window);
		//ImGui::ShowMetricsWindow();

// 		static EditorTools::NodeGraph nodeGraph(PythonGraph::nodeTypes, PythonGraph::CreateNode);
// 		if (ImGui::Begin("Node Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
// 			nodeGraph.RenderNodeGraph();
// 		}
// 		ImGui::End();
		
		/*static EditorTools::ShaderGraph shaderGraph;
		if (m_ShowNodeGraph) {
			if (ImGui::Begin("Shader Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				
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
		ImGui::End();
		
		m_SceneHeirarchyPanel.SetContext(m_Scene);
		m_SceneHeirarchyPanel.OnImGuiRender();
		auto entity = m_SceneHeirarchyPanel.GetSelectedEntity();
		m_InspectorPanel.SetContext(entity, m_Scene);
		m_InspectorPanel.OnImGuiRender();
		ShowLogger();

// 		if (ImGui::Button("Parse Graph")) {
// 			APEX_LOG_INFO("Node Graph Output:");
// 			APEX_LOG_INFO(nodeGraph.Parse().c_str());
// 		}

		ShowSettings();
		ShowGameViewport();
		
		EndDockspace();
		
		ImGui::PopFont();
	}

	void EditorLayer::BeginDockspace()
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
	}
	
	void EditorLayer::EndDockspace()
	{
		// Clear out Dockspace vars
		ImGui::End();
	}
	
	void EditorLayer::ShowSettings()
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
	
	void EditorLayer::ShowGameViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
		ImGui::Begin("Game View");
		
		m_ViewportFocussed = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer().SetBlockMouseEvents(!m_ViewportFocussed || !m_ViewportHovered);
		Application::Get().GetImGuiLayer().SetBlockKeyboardEvents(!m_ViewportFocussed);
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_GameViewportSize = *((glm::vec2*)&viewportSize);
		
		ImGui::Image((void*)(intptr_t)m_GameFramebuffer->GetColorAttachmentID(), { m_GameViewportSize.x, m_GameViewportSize.y }, { 0.f, 1.f }, { 1.f, 0.f });
		ImGui::End();
		ImGui::PopStyleVar();
	}
	
	void EditorLayer::ShowLogger()
	{
		static bool showLogLevel = false;
		static bool showSourceLocation = false;
		static bool showSourceFunction = false;
		static bool showThreadId = false;
		static bool showProcessId = false;
		m_LogPatternChanged = false;
		
		ImGui::Begin("Log");
		if (ImGui::BeginPopup("Options")) {
			if (ImGui::Checkbox("Log Level", &showLogLevel)) {
				m_LogPatternChanged = true;
				if (showLogLevel)
					m_LogPatternFlags |= LogPattern::LogLevel;
				else
					m_LogPatternFlags &= ~LogPattern::LogLevel;
			}
			if (ImGui::Checkbox("Source Location", &showSourceLocation)) {
				m_LogPatternChanged = true;
				if (showSourceLocation)
					m_LogPatternFlags |= LogPattern::SourceLocation;
				else
					m_LogPatternFlags &= ~LogPattern::SourceLocation;
			}
			if (ImGui::Checkbox("Source Function", &showSourceFunction)) {
				m_LogPatternChanged = true;
				if (showSourceFunction)
					m_LogPatternFlags |= LogPattern::SourceFunction;
				else
					m_LogPatternFlags &= ~LogPattern::SourceFunction;
			}
			if (ImGui::Checkbox("Thread ID", &showThreadId)) {
				m_LogPatternChanged = true;
				if (showThreadId)
					m_LogPatternFlags |= LogPattern::ThreadId;
				else
					m_LogPatternFlags &= ~LogPattern::ThreadId;
			}
			if (ImGui::Checkbox("Process ID", &showProcessId)) {
				m_LogPatternChanged = true;
				if (showProcessId)
					m_LogPatternFlags |= LogPattern::ProcessId;
				else
					m_LogPatternFlags &= ~LogPattern::ProcessId;
			}
			ImGui::EndPopup();
		}
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		
		static bool showInternalLogs = false;
		if (ImGui::Checkbox("Show Internal Logs", &showInternalLogs)) {
			if (showInternalLogs)
				Log::GetCoreLogger()->sinks().push_back(m_LogSink);
			else
				Log::GetCoreLogger()->sinks().pop_back();
		}
		ImGui::End();
		
		if (m_LogPatternChanged)
			m_LogSink->SetPatternFlags(m_LogPatternFlags);
		
		m_LogPanel.OnImGuiRender();
	}
	
	static ::SingleCopyStack<std::string, 16> s_RecentFiles;
	
	static void OpenFileStub(const std::string& filename)
	{
		APEX_LOG_INFO("File opened: {}", filename.c_str());
	}
	
	void EditorLayer::ShowMainMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				DrawFileMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				DrawEditMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("ImGui Demo"))
					show_imgui_demo_window = true;
				
				if (ImGui::MenuItem("Node Graph"))
					m_ShowNodeGraph = true;
				
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
	
	void EditorLayer::DrawFileMenu()
	{
		if (ImGui::MenuItem("New")) {
			
		}
		if (ImGui::MenuItem("Open", "Ctrl+O")) {
			auto filename = Utils::OpenFileDialog();
			if (!filename.empty()) {
				OpenFileStub(filename);

				auto scene = CreateRef<Scene>();
				auto serializer = SceneSerializerFactory().SetFormat(SceneSerializerFactory::Format::XML).Build(scene);
				if (serializer->Deserialize(filename)) {
					scene->OnSetup();
					m_Scene = scene;
				}

				s_RecentFiles.Push(filename);
			}
		}
		if (ImGui::BeginMenu("Open Recent", !s_RecentFiles.Empty())) {
			std::string selectedFile = "";
			for (std::string file : s_RecentFiles)
				if (ImGui::MenuItem(file.c_str())) {
					OpenFileStub(file);
					selectedFile = file;
				}
			if (!selectedFile.empty())
				s_RecentFiles.Push(selectedFile);
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {
			auto serializer = SceneSerializerFactory().SetFormat(SceneSerializerFactory::Format::XML).Build(m_Scene);
			serializer->Serialize("/assets/scene.xml");
		}
		if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {
			
		}
		if (ImGui::MenuItem("Exit")) {
			Application::Get().Close();
		}
	}
	
	void EditorLayer::DrawEditMenu()
	{
		static bool canUndo = false;
		static bool canRedo = false;
		
		if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canUndo)) {
			
		}
		if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", false, canRedo)) {
			
		}
		if (ImGui::MenuItem("Create New")) {
			m_Scene->CreateEntity();
		}
		if (ImGui::MenuItem("Preferences", "Ctrl+P")) {
			
		}
	}

	void EditorLayer::ShowComputeShaderOutput()
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
	
}
 
