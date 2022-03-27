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


#include "EditorFonts.h"
#include "IconsMaterialDesign.h"
#include "ImGuizmo.h"
#include "Apex/Core/DllManager.h"
#include "Apex/Core/ECS/Components/PhysicsComponents.h"
#include "Apex/Graphics/FBXImporter.h"
#include "Apex/Graphics/Material.h"
#include "Apex/Graphics/Font.h"
#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Utils/ScopeGuard.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	//TODO: Remove this after testing!
	static glm::vec3 lightPos { 1.0f, 5.6f, 2.0f };
	static glm::vec3 directionalLightDirection{ 1.f, -1.f, 1.f };

	Ref<FontAtlas> s_FontAtlas;
	
	EditorLayer::EditorLayer()
		: Layer("ApexEditor"),
		  m_BGColor{0.023f, 0.039f, 0.124f, 1.0f},
		  m_EditorCamera(Camera::ProjectionType::Perspective, Application::Get().GetWindow().GetWidth(),
		                 Application::Get().GetWindow().GetHeight()),
		  m_OrthographicCameraController(m_EditorCamera, 1.f, glm::vec3{0.f, 0.f, 5.f}),
		  m_PerspectiveCameraController(m_EditorCamera, {-5.f, 5.f, 5.f}),
		  m_EditorCameraController(&m_PerspectiveCameraController)
	{
		m_EditorCamera.SetOrthographicNear(0.1f);
		m_EditorCamera.SetOrthographicFar(1000.f);

		m_EditorCamera.SetPerspectiveNear(0.1f);
		m_EditorCamera.SetPerspectiveFar(1000.f);

		m_PerspectiveCameraController.LookAt({0.f, 0.f, 0.f});
		m_PerspectiveCameraController.MovementSpeed() = 1.0f;

		// Logger
		m_LogSink = std::make_shared<EditorLogSink_mt>(&m_LogPanel);
		Log::GetCoreLogger()->sinks().push_back(m_LogSink);
		Log::GetClientLogger()->sinks().push_back(m_LogSink);
		FileSystem::Mount("editor_assets", APEX_INSTALL_LOCATION "/assets");
	}

	void EditorLayer::OnAttach()
	{
		FileSystem::MountRoot(APEX_INSTALL_LOCATION "/assets");

		// TODO: Remove
		const auto dll = LoadDll(APEX_INSTALL_LOCATION "/../ScriptTest/ScriptTest.dll");

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		// TODO: Framebuffer builder - specify all framebuffer attachments before actually creating the framebuffer (avoid invalidating over and over)
		m_GameFramebuffer = Framebuffer::Create({ 1280u, 720u, 1, true});
		m_GameFramebufferMS = Framebuffer::Create({ 1280u, 720u, 0, false, true, false, false, 8 });
		m_GameFramebufferMS->AddColorAttachment({ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT, TextureFiltering::BILINEAR });
		m_PostProcessFramebuffer = Framebuffer::Create({ 1280u, 720u, 0, false });
		m_PostProcessFramebuffer->AddColorAttachment({ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA16, TextureDataType::FLOAT, TextureFiltering::BILINEAR });
		m_ShaderGrid = Shader::Create("editor_assets/shaders/InfiniteGridXZ.glsl");

		m_ShadowMap = CreateRef<ShadowMap>(2048u, 2048u);

		auto& resourceManager = Application::Get().GetResourceManager();
		/* Initialize Shaders */
		auto cubemapShader = resourceManager.Insert<Shader>(RESNAME("shader_Skybox"), "editor_assets/shaders/Skybox.glsl");
		auto tonemapShader = resourceManager.Insert<Shader>(RESNAME("shader_ACESTonemap"), "editor_assets/shaders/ACESTonemap.glsl");
		auto debugVerticesShader = resourceManager.Insert<Shader>(RESNAME("shader_DebugVerticesUnlit"), "editor_assets/shaders/DebugVerticesUnlit.glsl");
		auto debugTrianglesShader = resourceManager.Insert<Shader>(RESNAME("shader_DebugTrianglesUnlit"), "editor_assets/shaders/DebugTrianglesUnlit.glsl");
		auto albedoUnlitShader = resourceManager.Insert<Shader>(RESNAME("shader_AlbedoUnlit"), "editor_assets/shaders/AlbedoUnlit.glsl");
		auto standardPBRShader = resourceManager.Insert<Shader>(RESNAME("shader_StandardPBR"), "editor_assets/shaders/StandardPBR_new.glsl");

		resourceManager.Insert<Material>(RESNAME("material_DebugVertices"), CreateRef<Material>(debugVerticesShader));
		resourceManager.Insert<Material>(RESNAME("material_DebugTriangles"), CreateRef<Material>(debugTrianglesShader));
		resourceManager.Insert<Material>(RESNAME("material_AlbedoUnlit"), CreateRef<Material>(albedoUnlitShader));

		/* Initialize Textures */
		m_IconEditor = Texture2D::Create(APEX_INSTALL_LOCATION "/assets/Apex-Game-Engine-32.png");
		m_ImageTexture = Texture2D::Create(256U, 256U, HDRTextureSpec, "Image");
		m_ComputeShader = ComputeShader::Create("Blur.compute");

		/*auto cubemapTexture = TextureCubemap::Create({
			"editor_assets/textures/skyboxes/driving_school/px.png",
			"editor_assets/textures/skyboxes/driving_school/nx.png",
			"editor_assets/textures/skyboxes/driving_school/py.png",
			"editor_assets/textures/skyboxes/driving_school/ny.png",
			"editor_assets/textures/skyboxes/driving_school/pz.png",
			"editor_assets/textures/skyboxes/driving_school/nz.png",
		});*/
		auto cubemapTexture = TextureCubemap::Create({
			"editor_assets/textures/Checkerboard.png",
			"editor_assets/textures/Checkerboard.png",
			"editor_assets/textures/Checkerboard.png",
			"editor_assets/textures/Checkerboard.png",
			"editor_assets/textures/Checkerboard.png",
			"editor_assets/textures/Checkerboard.png",
		});
		resourceManager.Insert<Texture>(RESNAME("texture_Skybox"), cubemapTexture);
		//auto pusheenTexture = resourceManager.Insert<Texture>(RESNAME("pusheen-texture"), "editor_assets/textures/pusheen-thug-life.png");
		//auto suzanneTexture = resourceManager.Insert<Texture>(RESNAME("suzanne_DefaultMaterial_BaseColor"), "editor_assets/meshes/suzanne/textures/suzanne_DefaultMaterial_BaseColor.png");
		//auto metalPlateDiffuseTexture = resourceManager.Insert<Texture>(RESNAME("metal_plate_diff_2k"), Texture2D::Create("editor_assets/textures/metal_plate/metal_plate_diff_2k.jpg", true));

		/* Initialize Meshes */
		resourceManager.Insert<Mesh>(RESNAME("default-cube"),Primitives::Cube::GetMesh());
		resourceManager.Insert<Mesh>(RESNAME("default-plane"), Primitives::Plane::GetMesh());
		//auto suzanneMesh = resourceManager.Insert<Mesh>(RESNAME("suzanne-mesh"), "editor_assets/meshes/suzanne/source/suzanne.fbx");

		/* Initialize Materials */
		//auto _suzanneMaterial = CreateRef<Material>();
		//_suzanneMaterial->SetShader(albedoUnlitShader);
		//_suzanneMaterial->SetTexture("Albedo", suzanneTexture);
		//auto suzanneMaterial = resourceManager.Insert<Material>(RESNAME("suzanne_material_Unlit"), _suzanneMaterial);

		/* Load All Resources */
		resourceManager.LoadAllResources();

		/* Entity initialization */
		/*auto cubeEntity = m_ActiveScene->CreateEntity(HASH("cube"));
		cubeEntity.AddComponent<MeshRendererComponent>(cubeMesh.GetId(), debugTrianglesShader.GetId());

		auto planeEntity = m_ActiveScene->CreateEntity(HASH("plane"));
		planeEntity.AddComponent<MeshRendererComponent>(planeMesh.GetId(), debugTrianglesShader.GetId());
		planeEntity.GetComponent<TransformComponent>().scale *= glm::vec3(5.f, 1.f, 5.f);*/

		//auto suzanneEntity = m_ActiveScene->CreateEntity(HASH("suzanne"));
		//suzanneEntity.AddComponent<MeshRendererComponent>(suzanneMesh, suzanneMaterial);

		{
			auto planeEntity = m_ActiveScene->CreateEntity(HASH("plane"));
			planeEntity.Transform().scale *= glm::vec3{ 50.f, 1.f, 50.f };
			auto& meshComp = planeEntity.AddComponent<MeshRendererComponent>();
			meshComp.mesh = resourceManager.Get<Mesh>(RESNAME("default-plane"));
			meshComp.material = resourceManager.Insert<Material>(RESNAME("default-material"), CreateRef<Material>(resourceManager.Get<Shader>(RESNAME("shader_StandardPBR"))));
		}
		{
			auto sphereEntity = m_ActiveScene->CreateEntity(HASH("Sphere"));
			sphereEntity.Transform().translation.y += 15.f;
			auto& meshComp = sphereEntity.AddComponent<MeshRendererComponent>();
			auto mesh = FBXImporter::LoadMesh(FileSystem::GetFileIfExists("editor_assets/meshes/sphere-tris.fbx"), "Sphere");
			meshComp.mesh = resourceManager.Insert<Mesh>(RESNAME("default-sphere"), mesh);
			auto material = CreateRef<Material>(resourceManager.Get<Shader>(RESNAME("shader_StandardPBR")));
			meshComp.material = resourceManager.Insert<Material>(RESNAME("material.sphere"), material);

			sphereEntity.AddComponent<SphereCollider>();
		}
		{
			auto dirLightEntity = m_ActiveScene->CreateEntity(HASH("Directional Light"));
			dirLightEntity.Transform().rotation = glm::eulerAngles(glm::quatLookAt(glm::normalize(glm::vec3{ 1.f, 1.f, 0.f }), glm::vec3{ 0.f, 1.f, 0.f }));
			auto& lightComp = dirLightEntity.AddComponent<LightComponent>();
			lightComp.type = LightType::DirectionalLight;
			lightComp.color = glm::vec4{ 0.98f, 0.92f, 0.85f, 1.f };
			lightComp.intensity = 8.f;
		}
		
		m_ActiveScene->OnSetup();

		// Panels
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_AssetExplorer.OnAttach();
		m_AssetExplorer.SetContext("");
		m_ResourceViewer.OnAttach();

		// ImGui options
		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
		SetupEditorFonts();

		// FBXImporter::Import(FileSystem::GetFileIfExists("editor_assets/meshes/sphere-tris.fbx"), m_ActiveScene);
		// FBXImporter::Import(FileSystem::GetFileIfExists("editor_assets/meshes/suzanne/source/suzanne.fbx"), m_ActiveScene);

		//s_FontAtlas = CreateRef<FontAtlas>();
		//(void)s_FontAtlas->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 50.f);
		//s_FontAtlas->BuildRGBA32();

		//auto textEntity = m_ActiveScene->CreateEntity(HASH("text"));
		//textEntity.AddComponent<TextRendererComponent>(u8"Hello", s_FontAtlas->GetFontAtIndex(0), glm::vec4{ 10.f, 4.f, 2.f, 1.f });
		//textEntity.Transform().scale.x = 4.f;
		//textEntity.Transform().scale.y = 4.f;

		// TODO: Move to Play()
		// m_ActiveScene->OnPlay();
		resourceManager.LoadAll<Texture>();
		resourceManager.LoadAll<Mesh>();
		resourceManager.LoadAll<Shader>();
	}
	
	void EditorLayer::OnDetach() 
	{
		//m_Sound->drop();
		//m_SoundEngine->drop();
	}

	static bool useMSAA = true;
	static bool showSkybox = true;
	static bool usePostProcess = true;

	void EditorLayer::OnUpdate(Timestep ts) 
	{
		// Update Editor state
		if (m_SceneState == SceneState::Transition_StartPlay)
			OnScenePlay();
		else if (m_SceneState == SceneState::Transition_StopPlay)
			OnSceneStop();

		// Resize
		if (auto& fbSpec = m_GameFramebuffer->GetSpecification();
			m_GameViewportSize.x > 0.f && m_GameViewportSize.y > 0.f &&
			(fbSpec.width != m_GameViewportSize.x || fbSpec.height != m_GameViewportSize.y))
		{
			if (fbSpec.width < m_GameViewportSize.x || fbSpec.height < m_GameViewportSize.y) {
				m_GameFramebuffer->Resize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
				m_GameFramebufferMS->Resize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
				m_PostProcessFramebuffer->Resize((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
			}
			m_EditorCameraController->OnResize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));
			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_GameViewportSize.x), static_cast<uint32_t>(m_GameViewportSize.y));
		}
		
		if (m_ViewportFocused)
			m_EditorCameraController->OnUpdate(ts);

		// Update
		if (m_SceneState == SceneState::Play) {
			m_ActiveScene->OnUpdate(ts);
		}

		// Render
		// Set Camera Data


		// Depth Pre-Pass
		RenderCommands::SetCulling(true);
		RenderCommands::SetDepthTest(true);
		RenderCommands::SetClearColor(glm::vec4{1.f, 1.f, 1.f, 1.f});
		Renderer::SetupLights(m_ActiveScene);
		m_ShadowMap->SetupLightAndCamera(directionalLightDirection, m_EditorCamera, m_EditorCameraController->GetTransform());
		m_ShadowMap->BuildForScene(m_ActiveScene);

		Renderer2D::ResetStats();

		if (useMSAA)
			m_GameFramebufferMS->Bind();
		else
			m_GameFramebuffer->Bind();
		RenderCommands::SetClearColor(m_BGColor);
		RenderCommands::Clear();

		
		auto cameraTransform = m_EditorCameraController->GetTransform();
		auto cameraTranslation = cameraTransform[3];

		auto skyboxTexture = Application::Get().GetResourceManager().Get<Texture>(RESNAME("texture_Skybox"));
		skyboxTexture->Bind(1);

		// TODO:
		// 1. Transfer Camera and environment related variables to Uniform Buffer
		//    and set them once every frame in Renderer::BeginScene
		// 2. Transfer Light characteristics to dedicated Light class and Uniform Buffer
		//    and use those objects to set shader variables inside Renderer::BeginScene
		auto pbrShader = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_StandardPBR"));
		//pbrShader->SetUniFloat3("u_CameraPosition", m_EditorCameraController->GetTransform()[3]);
		pbrShader->SetUniFloat3("u_LightPos", lightPos);
		pbrShader->SetUniFloat3("u_LightDir", directionalLightDirection);
		pbrShader->SetUniMat4("u_LightSpaceTransform", m_ShadowMap->GetLightSpaceTransform());
		m_ShadowMap->GetDepthTexture()->Bind(5);

		RenderCommands::SetCulling(true);

		Renderer::BeginScene(m_EditorCamera, m_EditorCameraController->GetTransform());
		RenderCommands::SetDepthTest(true);
		m_ActiveScene->Render3D();
		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnScriptRender(ts);
		Renderer::EndScene();

		if (showSkybox) {
			auto skyboxShader = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_Skybox"));
			skyboxShader->Bind();
			cameraTransform[3] = glm::vec4(0, 0, 0, 1);
			skyboxShader->SetUniMat4("u_Projection", m_EditorCamera.GetProjection());
			skyboxShader->SetUniMat4("u_View", glm::transpose(cameraTransform));
			RenderCommands::SetDepthTestFunction(DepthStencilMode::LEQUAL);
			RenderCommands::Draw(6);
			RenderCommands::SetCulling(true);
			RenderCommands::SetDepthTestFunction(DepthStencilMode::LESS);
		}

		RenderCommands::SetCulling(false);
		Renderer2D::BeginScene(m_EditorCamera, m_EditorCameraController->GetTransform());
		m_ActiveScene->Render2D();
		Renderer2D::EndScene();

		// Render the Grid
		cameraTransform[3] = cameraTranslation;
		glm::mat4 gridTransform = glm::mat4(1.f);
		gridTransform[3] = glm::vec4(cameraTranslation.x, 0.f, cameraTranslation.z, 1.f);

		// m_ShaderGrid = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_InfiniteGridXZ"));
		m_ShaderGrid->Bind();
		m_ShaderGrid->SetUniMat4("u_ViewProjection", m_EditorCamera.GetProjection() * glm::inverse(cameraTransform));
		m_ShaderGrid->SetUniMat4("u_ModelMatrix", gridTransform);
		RenderCommands::SetCulling(false);
		RenderCommands::Draw(6);

		// Post process
		if (useMSAA) {
			m_GameFramebufferMS->Blit(m_PostProcessFramebuffer);
		} else {
			m_GameFramebuffer->Blit(m_PostProcessFramebuffer);
		}

		if (usePostProcess) {
			m_GameFramebuffer->Bind();
			RenderCommands::Clear();
			auto tonemapShader = Application::Get().GetResourceManager().Get<Shader>(RESNAME("shader_ACESTonemap"));
			tonemapShader->Bind();
			m_PostProcessFramebuffer->GetColorAttachment(0)->Bind(1);
			RenderCommands::Draw(3);
		} else {
			m_PostProcessFramebuffer->Blit(m_GameFramebuffer);
		}
		
		m_GameFramebuffer->Unbind();
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_ResourceManagerSnapshot = CreateRef<ResourceManager>();
		Application::Get().GetResourceManager().CreateSnapshot(*m_ResourceManagerSnapshot);

		m_ActiveScene = m_EditorScene->Copy();
		m_ActiveScene->OnPlay();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_InspectorPanel.SetContext({}, m_ActiveScene);
		if (m_MaterialPanel.GetContext().IsValid()) {
			m_MaterialPanel.SetContext(m_MaterialPanel.GetContext().GetId());
		}
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_ActiveScene->OnStop();
		m_ActiveScene = m_EditorScene;

		Application::Get().GetResourceManager().LoadSnapshot(*m_ResourceManagerSnapshot);
		m_ResourceManagerSnapshot = nullptr;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_InspectorPanel.SetContext({}, m_ActiveScene);
		if (m_MaterialPanel.GetContext().IsValid()) {
			auto id = m_MaterialPanel.GetContext().GetId();
			if (Application::Get().GetResourceManager().Exists(id))
				m_MaterialPanel.SetContext(id);
			else
				m_MaterialPanel.SetContext({});
		}
	}

	static bool show_imgui_demo_window = false;
	
	void EditorLayer::OnImGuiRender()
	{
		MainMenuBar();
		BeginDockspace();

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
			ImGui::Checkbox("MSAA", &useMSAA);
			ImGui::Checkbox("Post process", &usePostProcess);
			ImGui::Checkbox("Show Skybox", &showSkybox);
			ImGui::Separator();

			ImGui::BeginGroup();
			ImGui::DragFloat3("Light Pos", &lightPos.x);
			ImGui::DragFloat3("Light Dir", &directionalLightDirection.x);
			ImGui::EndGroup();

			ImGui::Separator();

			static int gizmoType = 0;
			if (ImGui::Combo("Gizmo Mode", &gizmoType, "None\0Translate\0Rotate\0Scale\0")) {
				switch (gizmoType) {
				case 0:
					m_GizmoType = -1;
					break;
				case 1:
					m_GizmoType = (int)ImGuizmo::OPERATION::TRANSLATE;
					break;
				case 2:
					m_GizmoType = (int)ImGuizmo::OPERATION::ROTATE;
					break;
				case 3:
					m_GizmoType = (int)ImGuizmo::OPERATION::SCALE;
					break;
				default: ;
				}
			}

			if (ImGui::Button("Recompile Shaders")) {
				Application::Get().GetResourceManager().LoadAll<Shader>();
			}

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

					ImGui::Text(fmt::format("Transform: {0}", Math::ParseMatrix(m_EditorCameraController->GetTransform())).c_str());

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

		Logger();
		m_AssetExplorer.OnImGuiRender();
		m_MaterialPanel.OnImGuiRender();
		m_ResourceViewer.OnImGuiRender();
		m_SceneHierarchyPanel.OnImGuiRender();
		auto entity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (entity)
			m_InspectorPanel.SetContext(entity, m_ActiveScene);
		m_InspectorPanel.OnImGuiRender();

		SceneToolbar();

// 		if (ImGui::Button("Parse Graph")) {
// 			APEX_LOG_INFO("Node Graph Output:");
// 			APEX_LOG_INFO(nodeGraph.Parse().c_str());
// 		}

		ShowSettings();
		GameViewport();

		/*if (ImGui::Begin("Depth Texture", nullptr, ImGuiWindowFlags_NoScrollbar)) {
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_ShadowMap.GetDepthTexture()->GetID())),
				{ 200, 200 }, {0.f, 1.f}, {1.f, 0.f});
		}
		ImGui::End();*/

		EndDockspace();
	}

	void EditorLayer::BeginDockspace()
	{
		ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking
		//	| ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
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
	
	void EditorLayer::GameViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
		ImGui::Begin("Game View");
		
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer().SetBlockMouseEvents(!m_ViewportFocused || !m_ViewportHovered);
		Application::Get().GetImGuiLayer().SetBlockKeyboardEvents(!m_ViewportFocused);
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowMin = ImGui::GetWindowContentRegionMin();
		m_GameViewportSize = *reinterpret_cast<glm::vec2*>(&viewportSize);
		m_GameViewportPos = { windowPos.x + windowMin.x, windowPos.y + windowMin.y };

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_GameFramebuffer->GetColorAttachmentID())),
		             {m_GameViewportSize.x, m_GameViewportSize.y}, {0.f, 1.f}, {1.f, 0.f});

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM")) {
				auto path = fs::path(static_cast<const char*>(payload->Data));
				if (path.extension() == ".fbx") {
					FBXImporter::Import(FileSystem::GetFileIfExists(path), m_ActiveScene);
				} else if (path.extension() == ".apx") {
					SceneOpen(path);
				}
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

		DrawGizmos();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::DrawGizmos()
	{
		Entity entity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (entity && m_GizmoType != -1) {
			//ImGuizmo::SetOrthographic(m_EditorCamera.GetProjectionType() == Camera::ProjectionType::Orthographic);
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_GameViewportPos.x, m_GameViewportPos.y, m_GameViewportSize.x, m_GameViewportSize.y);

			// Camera
			auto& cameraProjection = m_EditorCamera.GetProjection();
			auto cameraView = glm::inverse(m_EditorCameraController->GetTransform());

			// Entity transform
			auto& transComp = entity.Transform();
			auto transform = transComp.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			                     static_cast<ImGuizmo::OPERATION>(m_GizmoType), ImGuizmo::MODE::WORLD, glm::value_ptr(transform));

			if ( ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - transComp.rotation;
				transComp.translation = translation;
				transComp.rotation += deltaRotation;
				transComp.scale = scale;
			}
		}
	}

	void EditorLayer::Logger()
	{
		static bool showLogLevel = false;
		static bool showSourceLocation = false;
		static bool showSourceFunction = false;
		static bool showThreadId = false;
		static bool showProcessId = false;
		m_LogPatternChanged = false;

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<uint32_t>(EditorFontIndex::UI)]);
		ON_SCOPE_END { ImGui::PopFont(); };
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
	
	void EditorLayer::MainMenuBar()
	{
		const auto origFramePadding = ImGui::GetStyle().FramePadding;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { origFramePadding.x + 4.f, origFramePadding.y + 3.f });
		const auto origWindowPadding = ImGui::GetStyle().WindowPadding;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, origWindowPadding.y });

		if (ImGui::BeginMainMenuBar())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, origFramePadding);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, origWindowPadding);

			auto imgSize = ImGui::GetWindowSize().y;// - ImGui::GetStyle().WindowPadding.y;
			ImGui::Image((void*)(intptr_t)m_IconEditor->GetID(), ImVec2{ imgSize, imgSize });
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

			ImGui::PopStyleVar(2);

			{
				const auto _origFramePadding = ImGui::GetStyle().FramePadding;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { _origFramePadding.x + 14.f, _origFramePadding.y + 2.f });
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_MenuBarBg));
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[(uint32_t)EditorFontIndex::Window]);
				ON_SCOPE_END {
					ImGui::PopFont();
					ImGui::PopStyleColor(1);
					ImGui::PopStyleVar();
				};

				auto& style = ImGui::GetStyle();

				const float close_btn_size = ImGui::CalcTextSize(ICON_MD_CLOSE).x + style.FramePadding.x * 2.f;
				float cursorPosX = ImGui::GetContentRegionMax().x - close_btn_size;

				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff0000ff);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xff0000ee);
				ImGui::SetCursorPosX(cursorPosX);
				if (ImGui::Button(ICON_SEG_CLOSE)) {
					Application::Get().Close();
				}
				ImGui::PopStyleColor(2);

				cursorPosX -= close_btn_size;
				ImGui::SetCursorPosX(cursorPosX);
				if (Application::Get().GetWindow().IsMaximized()) {
					if (ImGui::Button(ICON_SEG_RESTORE)) {
						Application::Get().GetWindow().WindowRestore();
					}
				} else {
					if (ImGui::Button(ICON_SEG_MAXIMIZE)) {
						Application::Get().GetWindow().WindowMaximize();
					}
				}

				cursorPosX -= close_btn_size;
				ImGui::SetCursorPosX(cursorPosX);
				if (ImGui::Button(ICON_SEG_MINIMIZE)) {
					Application::Get().GetWindow().WindowMinimize();
				}
			}

			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar(2);
	}

	void EditorLayer::SceneToolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::Begin("##scene-toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::Button(m_SceneState == SceneState::Edit ? ICON_MD_PLAY_ARROW : ICON_MD_STOP))
		{
			if (m_SceneState == SceneState::Edit)
				m_SceneState = SceneState::Transition_StartPlay;
			else if (m_SceneState == SceneState::Play)
				m_SceneState = SceneState::Transition_StopPlay;
		}
		ImGui::End();

		ImGui::PopStyleVar(2);
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
		if (ImGui::MenuItem("Exit", "Alt+F4")) {
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
			m_ActiveScene->CreateEntity();
		}
		if (ImGui::MenuItem("Preferences", "Ctrl+P")) {
			
		}
	}

	void EditorLayer::ComputeShaderOutput()
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
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
			m_EditorScene = std::move(scene);
			m_ActiveScene = m_EditorScene;
			m_RecentFiles.Push(path.string());
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			m_InspectorPanel.SetContext({}, m_ActiveScene);
		}

	}

	void EditorLayer::SceneSave()
	{
		auto filename = m_RecentFiles.Empty() ? Utils::SaveFileDialog() : m_RecentFiles.Top();
		if (!filename.empty()) {
			SceneSaveAs(filename);
		}
	}

	void EditorLayer::SceneSaveAs()
	{
		auto filename = m_RecentFiles.Empty() ? Utils::SaveFileDialog() : Utils::SaveFileDialog(m_RecentFiles.Top());
		if (!filename.empty()) {
			SceneSaveAs(filename);
		}
	}

	void EditorLayer::SceneSaveAs(const fs::path& path) const
	{
		auto serializer = SceneSerializerFactory().SetFormat(SceneSerializerFactory::Format::XML).Build(m_ActiveScene);
		serializer->Serialize(path);
		APEX_LOG_INFO("Saved scene to file '{}'", path);
	}

}
 
