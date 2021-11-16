#include <apexed_pch.h>
#include <Apex.h>

#include "EditorLayer.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Utils/CustomDataStructures.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Core/ECS/SceneSerializer.h"

#include "Panels/PieMenu.h"
#include "Primitives.h"
// #include "EditorTools/NodeGraph/Node.h"
// #include "EditorTools/NodeGraph/NodeGraph.h"
 //#include "EditorTools/PythonGraph/PythonGraph.h"
 //#include "EditorTools/ShaderGraph/ShaderGraph.h"


#include "Apex/Graphics/Material.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Apex {
	
	EditorLayer::EditorLayer()
		: Layer("ApexEditor"), m_BGColor{0.42f, 0.63f, 0.75f, 1.0f},
		m_EditorCamera(Camera::ProjectionType::Perspective, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight()),
		m_OrthographicCameraController(m_EditorCamera, 1.f, glm::vec3{ 0.f, 0.f, 5.f }),
		m_PerspectiveCameraController(m_EditorCamera, { -5.f, 5.f, 5.f }),
		m_EditorCameraController(&m_PerspectiveCameraController)
	{
		m_EditorCamera.SetOrthographicNear(0.f);
		m_EditorCamera.SetOrthographicFar(10000.f);

		m_PerspectiveCameraController.LookAt({ 0.f, 0.f, 0.f });
		m_PerspectiveCameraController.MovementSpeed() = 1.0f;

		// Logger
		m_LogSink = std::make_shared<EditorLogSink_mt>(&m_LogPanel);
		Log::GetCoreLogger()->sinks().push_back(m_LogSink);
		Log::GetClientLogger()->sinks().push_back(m_LogSink);
		FileSystem::Mount("editor_assets", APEX_INSTALL_LOCATION "/assets");
		Application::Get().GetWindow().SetWindowIcon(Apex::Utils::LoadImage(FileSystem::GetFileIfExists("editor_assets/Apex-Game-Engine-32.png")));
	}

	void EditorLayer::OnAttach()
	{
		FileSystem::MountRoot(APEX_INSTALL_LOCATION "/assets");

		m_Scene = CreateRef<Scene>();
		m_GameFramebuffer = Framebuffer::Create({ 1280u, 720u });

		auto& resourceManager = Application::Get().GetResourceManager();

		/* Initialize Shaders */
		auto gridShader = resourceManager.AddResourceFromFile<Shader>(RESNAME("shader_InfiniteGridXZ"), "editor_assets/shaders/InfiniteGridXZ.glsl");
		auto debugVerticesShader = resourceManager.AddResourceFromFile<Shader>(RESNAME("shader_DebugVerticesUnlit"), "editor_assets/shaders/DebugVerticesUnlit.glsl");
		auto debugTrianglesShader = resourceManager.AddResourceFromFile<Shader>(RESNAME("shader_DebugTrianglesUnlit"), "editor_assets/shaders/DebugTrianglesUnlit.glsl");
		auto albedoUnlitShader = resourceManager.AddResourceFromFile<Shader>(RESNAME("shader_AlbedoUnlit"), "editor_assets/shaders/AlbedoUnlit.glsl");

		/*resourceManager.Load(gridShader.GetId());
		resourceManager.Load(debugVerticesShader.GetId());
		resourceManager.Load(debugTrianglesShader.GetId());
		resourceManager.Load(unlitMaterial.GetId());*/

		/* Initialize Textures */
		m_ImageTexture = Texture2D::Create(256U, 256U, HDRTextureSpec, "Image");
		m_ComputeShader = ComputeShader::Create("Blur.compute");

		auto pusheenTexture = resourceManager.AddResourceFromFile<Texture>(RESNAME("pusheen-texture"), "editor_assets/textures/pusheen-thug-life.png");
		auto suzanneTexture = resourceManager.AddResourceFromFile<Texture>(RESNAME("suzanne_DefaultMaterial_BaseColor"), "editor_assets/meshes/suzanne/textures/suzanne_DefaultMaterial_BaseColor.png");

		/* Initialize Meshes */
		auto cubeMesh = resourceManager.AddResource<Mesh>(RESNAME("default-cube"),Primitives::Cube::GetMesh());
		auto planeMesh = resourceManager.AddResource<Mesh>(RESNAME("default-plane"), Primitives::Plane::GetMesh());
		auto suzanneMesh = resourceManager.AddResourceFromFile<Mesh>(RESNAME("suzanne-mesh"), "editor_assets/meshes/suzanne/source/suzanne.fbx");

		/* Initialize Materials */
		auto _suzanneMaterial = CreateRef<Material>();
		_suzanneMaterial->SetShader(albedoUnlitShader);
		_suzanneMaterial->AddTexture("Albedo", suzanneTexture);
		auto suzanneMaterial = resourceManager.AddResource<Material>(RESNAME("material_Unlit"), _suzanneMaterial);

		/* Load All Resources */
		resourceManager.LoadAllResources();

		/* Entity initialization */
		/*auto cubeEntity = m_Scene->CreateEntity(HASH("cube"));
		cubeEntity.AddComponent<MeshRendererComponent>(cubeMesh.GetId(), debugTrianglesShader.GetId());

		auto planeEntity = m_Scene->CreateEntity(HASH("plane"));
		planeEntity.AddComponent<MeshRendererComponent>(planeMesh.GetId(), debugTrianglesShader.GetId());
		planeEntity.GetComponent<TransformComponent>().scale *= glm::vec3(5.f, 1.f, 5.f);*/

		auto suzanneEntity = m_Scene->CreateEntity(HASH("suzanne"));
		suzanneEntity.AddComponent<MeshRendererComponent>(suzanneMesh, suzanneMaterial);

		// pusheenTexture->Bind(0);
		
		m_Scene->OnSetup();

		// Panels
		m_SceneHierarchyPanel.SetContext(m_Scene);
		m_AssetExplorer.OnAttach();
		m_AssetExplorer.SetContext("");

		// ImGui options
		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
		ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(font_cousine_compressed_data_base85, 12);
		ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/consola.ttf", 12);
#ifdef APEX_PLATFORM_WINDOWS
		auto font_cfg = ImFontConfig();
		font_cfg.OversampleH = 7;
		font_cfg.OversampleV = 5;
		auto font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 16.f, &font_cfg);
#endif
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
			m_EditorCameraController->OnResize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));
			m_Scene->OnViewportResize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));
		}
		
		if (m_ViewportFocused)
			m_EditorCameraController->OnUpdate(ts);
		
		
		// Render
		Renderer2D::ResetStats();
		
		m_GameFramebuffer->Bind();
		RenderCommands::SetClearColor(m_BGColor);
		RenderCommands::Clear();
		
		
		if (!m_PlayScene) {
			Renderer::BeginScene(m_EditorCamera, m_EditorCameraController->GetTransform());
			RenderCommands::SetDepthTest(true);
			m_Scene->Render3D();
			Renderer::EndScene();
			Renderer2D::BeginScene(m_EditorCamera, m_EditorCameraController->GetTransform());
			m_Scene->Render2D();
			Renderer2D::EndScene();


			// Render the Grid
			auto cameraTransform = m_EditorCameraController->GetTransform();
			glm::mat4 gridTransform = glm::mat4(1.f);
			gridTransform[3] = glm::vec4(cameraTransform[3].x, 0.f, cameraTransform[3].z, 1.f);

			auto gridShader = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_InfiniteGridXZ"));
			gridShader->Bind();
			gridShader->SetUniMat4("u_ViewProjection", m_EditorCamera.GetProjection() * glm::inverse(cameraTransform));
			gridShader->SetUniMat4("u_ModelMatrix", gridTransform);
			RenderCommands::SetCulling(false);
			RenderCommands::Draw(6);
			RenderCommands::SetCulling(true);
		} else {
			m_Scene->OnUpdate(ts);
		}
		
		m_GameFramebuffer->Unbind();
	}
	
	static bool show_imgui_demo_window = false;
	
	void EditorLayer::OnImGuiRender()
	{
		auto font2 = ImGui::GetIO().Fonts->Fonts[2];
		ImGui::PushFont(font2);
		BeginDockspace();
		ShowMainMenuBar();
		
		if (show_imgui_demo_window) ImGui::ShowDemoWindow(&show_imgui_demo_window);
		//ImGui::ShowMetricsWindow();

 		/*static EditorTools::NodeGraph nodeGraph(PythonGraph::nodeTypes, PythonGraph::CreateNode);
 		if (ImGui::Begin("Node Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
 			nodeGraph.RenderNodeGraph();
 		}
 		ImGui::End();*/
		
		/*static EditorTools::ShaderGraph shaderGraph;
		if (m_ShowNodeGraph) {
			if (ImGui::Begin("Shader Graph", &m_ShowNodeGraph, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
				
			}
			ImGui::End();
		}*/

		if (ImGui::Begin("Rendering")) {
			if (ImGui::Button("Import")) {
				ImGui::OpenPopup("Import Asset");
			}

			if (ImGui::IsPopupOpen("Import Asset")) {
				ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0.1f);
			}
			if (ImGui::BeginPopupModal("Import Asset", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
				ImGui::Text("Importing asset");
				if (ImGui::Button("Continue"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();

				ImGui::PopStyleVar();
			}

			ImGui::DragFloat4("Background", &m_BGColor[0], 0.001f, 0.0f, 1.0f);
			{
				ImGui::Separator();
				ImGui::Checkbox("Play", &m_PlayScene);
				ImGui::Separator();
				if (ImGui::TreeNode("Camera")) {
					constexpr const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
					constexpr const size_t projectionTypesLen = std::size(projectionTypeStrings);
					const char* currentProjectionTypeStr = projectionTypeStrings[(int)m_EditorCamera.GetProjectionType()];
					if (ImGui::BeginCombo("Projection", currentProjectionTypeStr)) {
						for (int i=0; i<projectionTypesLen; i++) {
							bool isSelected = projectionTypeStrings[i] == currentProjectionTypeStr;
							if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
								currentProjectionTypeStr = projectionTypeStrings[i];
								m_EditorCamera.SetProjectionType(static_cast<Camera::ProjectionType>(i));
							}
							
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						
						ImGui::EndCombo();
					}

					ImGui::Text(fmt::format("Transform: {0}", MathParser::ParseMatrix(m_EditorCameraController->GetTransform())).c_str());

					if (m_EditorCamera.GetProjectionType() == Camera::ProjectionType::Perspective) {
						float perspFov = glm::degrees(m_EditorCamera.GetPerspectiveVerticalFov());
						if (ImGui::DragFloat("Vertical FOV", &perspFov))
							m_EditorCamera.SetPerspectiveVerticalFov(glm::radians(perspFov));
						
						float perspNear = m_EditorCamera.GetPerspectiveNear();
						if (ImGui::DragFloat("Near", &perspNear))
							m_EditorCamera.SetPerspectiveNear(perspNear);
						
						float perspFar = m_EditorCamera.GetPerspectiveFar();
						if (ImGui::DragFloat("Far", &perspFar))
							m_EditorCamera.SetPerspectiveFar(perspFar);
					}
					else {
						float orthoSize = m_EditorCamera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &orthoSize))
							m_EditorCamera.SetOrthographicSize(orthoSize);
						
						float orthoNear = m_EditorCamera.GetOrthographicNear();
						if (ImGui::DragFloat("Near", &orthoNear))
							m_EditorCamera.SetOrthographicNear(orthoNear);
						
						float orthoFar = m_EditorCamera.GetOrthographicFar();
						if (ImGui::DragFloat("Far", &orthoFar))
							m_EditorCamera.SetOrthographicFar(orthoFar);
					}
					
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();

		// m_SceneHierarchyPanel.SetContext(m_Scene);
		m_SceneHierarchyPanel.OnImGuiRender();
		auto entity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (entity)
			m_InspectorPanel.SetContext(entity, m_Scene);
		m_InspectorPanel.OnImGuiRender();
		m_AssetExplorer.OnImGuiRender();

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ShowLogger();
		ImGui::PopFont();

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
		
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer().SetBlockMouseEvents(!m_ViewportFocused || !m_ViewportHovered);
		Application::Get().GetImGuiLayer().SetBlockKeyboardEvents(!m_ViewportFocused);
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_GameViewportSize = *reinterpret_cast<glm::vec2*>(&viewportSize);

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_GameFramebuffer->GetColorAttachmentID())),
		             {m_GameViewportSize.x, m_GameViewportSize.y}, {0.f, 1.f}, {1.f, 0.f});

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
				const char* path = static_cast<const char*>(payload->Data);
				SceneOpen(path);
			}

			ImGui::EndDragDropTarget();
		}

		auto curWinMin = ImGui::GetWindowContentRegionMin();
		curWinMin.x += ImGui::GetWindowPos().x + 10.f;
		curWinMin.y += ImGui::GetWindowPos().y + 10.f;
		ImGui::SetNextWindowPos(curWinMin);
		auto curWinWidth = ImGui::GetWindowContentRegionWidth();
		
		ImGui::BeginChild("##camera_controls", ImVec2(curWinWidth, 50.f), false, ImGuiWindowFlags_NoBackground);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 0.2f));

		if (m_EditorCameraController == &m_PerspectiveCameraController) {
			if (ImGui::Button("Ortho")) {
				m_EditorCameraController = &m_OrthographicCameraController;
				m_EditorCamera.SetProjectionType(Camera::ProjectionType::Orthographic);
			}
		}
		else if (m_EditorCameraController == &m_OrthographicCameraController) {
			if (ImGui::Button("Persp")) {
				m_EditorCameraController = &m_PerspectiveCameraController;
				m_EditorCamera.SetProjectionType(Camera::ProjectionType::Perspective);
			}
		}

		ImGui::PopStyleColor();
		ImGui::EndChild();

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
		
		/*static bool showInternalLogs = false;
		if (ImGui::Checkbox("Show Internal Logs", &showInternalLogs)) {
			if (showInternalLogs)
				Log::GetCoreLogger()->sinks().push_back(m_LogSink);
			else
				Log::GetCoreLogger()->sinks().pop_back();
		}*/
		ImGui::End();
		
		if (m_LogPatternChanged)
			m_LogSink->SetPatternFlags(m_LogPatternFlags);
		
		m_LogPanel.OnImGuiRender();
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
			SceneNew();
		}
		if (ImGui::MenuItem("Open", "Ctrl+O")) {
			SceneOpen();
		}
		if (ImGui::BeginMenu("Open Recent", !m_RecentFiles.Empty())) {
			for (std::string file : m_RecentFiles)
				if (ImGui::MenuItem(file.c_str())) {
					SceneOpen(file);
				}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {
			SceneSave();
		}
		if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S")) {
			SceneSaveAs();
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

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCameraController->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(APEX_BIND_EVENT_FN(OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(APEX_BIND_EVENT_FN(OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool mod_ctrl = Input::IsKeyPressed(APEX_KEY_LEFT_CONTROL) || Input::IsKeyPressed(APEX_KEY_RIGHT_CONTROL);
		bool mod_shift = Input::IsKeyPressed(APEX_KEY_LEFT_SHIFT) || Input::IsKeyPressed(APEX_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
		case APEX_KEY_N:
			if (mod_ctrl)
				SceneNew();
			break;
		case APEX_KEY_O:
			if (mod_ctrl)
				SceneOpen();
			break;
		case APEX_KEY_S:
			if (mod_ctrl && mod_shift) {
				SceneSaveAs();
			}
			else if (mod_ctrl) {
				SceneSave();
			}
			else if (!mod_shift) {
				// Set gizmo to Scale
			}
			break;
		case APEX_KEY_M:
			// Set gizmo to Translate/Move
			break;
		case APEX_KEY_R:
			// Set gizmo to Rotate
			break;

		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		return false;
	}

	void EditorLayer::SceneNew()
	{
		m_Scene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetContext(m_Scene);
	}

	void EditorLayer::SceneOpen()
	{
		auto filename = Utils::OpenFileDialog();
		if (!filename.empty()) {
			SceneOpen(filename);
		}
	}

	void EditorLayer::SceneOpen(const fs::path& path)
	{
		auto scene = CreateRef<Scene>();
		auto serializer = SceneSerializerFactory().SetFormat(SceneSerializerFactory::Format::XML).Build(scene);
		if (serializer->Deserialize(path.string())) {
			scene->OnSetup();
			m_Scene = scene;
			m_RecentFiles.Push(path.string());
			m_SceneHierarchyPanel.SetContext(m_Scene);
			m_InspectorPanel.SetContext({}, scene);
		}

	}

	void EditorLayer::SceneSave()
	{
		if (!m_RecentFiles.Empty()) {
			auto serializer = SceneSerializerFactory().SetFormat(SceneSerializerFactory::Format::XML).Build(m_Scene);
			serializer->Serialize(m_RecentFiles.Top());
			APEX_LOG_INFO("Saved scene to file '{}'", m_RecentFiles.Top());
		}
	}

	void EditorLayer::SceneSaveAs()
	{
	}

	void EditorLayer::SceneSaveAs(const fs::path& path)
	{
	}
	
}
 
