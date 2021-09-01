#include <Apex.h>
#include <imgui.h>

// #include <future>
// #include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

// #include "Apex/Graphics/Material.h"
#include "Apex/Graphics/Model.h"
#include "Apex/Graphics/Renderer/ForwardRenderer.h"
//#include "Apex/Utils/ComputeShader/ComputeShader.h"
//#include "Apex/Physics/ParticleSystem/ParticleSystem2D.h"

//#include "Games/DXBall/DXBall.h"

// Networking
//#include "Apex/Networking/NetworkManager.h"

// Audio
//#include "irrKlang.h"

#include "Apex/Core/FileSystem/VFS.h"

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Core/CameraController.h"

#include <imGuIZMOquat.h>

/****************      Example Layers      *****************/
// #include "TestLayer.h"
// #include "ParticleSystemExample.h"
// #include "ModelLoaderExample.h"
// #include "TerminalExample.h"

#if 1
class SandboxLayer: public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Model Viewer"), m_Camera(63.5f, 16.f / 9.f)
	{}
	
	~SandboxLayer() {}
	
	void OnAttach() override
	{
		Apex::FileSystem::Mount("/assets", APEX_INSTALL_LOCATION "/assets");
		
		m_PhongShader = Apex::Shader::Create("/core_assets/shaders/Phong.glsl");
		m_PhongNormalShader = Apex::Shader::Create("/core_assets/shaders/Phong-Normal.glsl");
		
		for (auto& modelFile : m_ModelFilePaths) {
			m_Models.push_back(Apex::Model::LoadModel(modelFile, false));
			ModelProps props{};
			props.Position = { Apex::Random::Float() * 10.f, 0.f, -15.f };
			m_Props.emplace_back(props);
		}
		
// 		m_Textures.push_back({
// 			Apex::Texture2D::Create("/assets/models/concrete-silo/silo_DefaultMaterial_BaseColor.png"),
// 			Apex::Texture2D::Create("/assets/models/concrete-silo/silo_DefaultMaterial_Roughness.png"),
// 			Apex::Texture2D::Create("/assets/models/concrete-silo/silo_DefaultMaterial_Normal.png"),
// 		});
// 		m_Textures.push_back({ Apex::Texture2D::Create("/assets/textures/Checkerboard.png") });
// 		m_Textures.push_back({
// 			Apex::Texture2D::Create("/assets/models/revolver/Rhino_Rhino_BaseColor.png"),
// 			Apex::Texture2D::Create("/assets/models/revolver/Rhino_Rhino_Roughness.png"),
// 			Apex::Texture2D::Create("/assets/models/revolver/Rhino_Rhino_Normal.png"),
// 		});
		
		m_WhiteTexture = Apex::Texture2D::Create(1, 1, Apex::SimpleTextureSpec);
		uint32_t whiteTextureData = 0xedededff;
		m_WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));
		
		m_Monkey = Apex::Model::LoadModel("/assets/models/blender-monkey.fbx", false);
	}
	
	void OnDetach() override
	{
	}
	
	void OnUpdate(Apex::Timestep ts) override
	{
		Apex::RenderCommands::SetClearColor({ 0.68f, 0.285f, 0.76f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::RenderCommands::SetDepthTest(true);
		Apex::RenderCommands::SetCulling(false);
		
		Apex::ForwardRenderer::BeginScene(m_Camera);
		Apex::ForwardRenderer::SetupLights();
		
// 		for (auto i=0; i<m_Models.size(); i++) {
// 			if (!m_Props[i].Show) continue;
// 			
// 			auto modelTransform = glm::translate(glm::mat4(1.f), m_Props[i].Position)
// 								* glm::mat4_cast(m_Props[i].Rotation)
// 								* glm::scale(glm::mat4(1.f), glm::vec3(m_Props[i].Scale));
// 			
// 			for (auto t=0; t<m_Textures[i].size(); t++)
// 				m_Textures[i][t]->Bind(t);
// 			
// 			if (!m_Props[i].UseAlbedoTexture)
// 				m_WhiteTexture->Bind(0);
// 			
// 			for (auto& [id, mesh] : m_Models[i]->GetMeshes()) {
// 				Apex::ForwardRenderer::SubmitMesh(mesh, (mesh->HasTangents() && m_Props[i].UseNormalTexture) ? m_PhongNormalShader : m_PhongShader, modelTransform);
// 				Apex::ForwardRenderer::SubmitMesh(mesh, m_PhongShader, modelTransform);
// 			}
// 		}
		
		for (uint32_t i=0; i<m_Rows; i++) for (uint32_t j=0; j<m_Rows; j++) {
			glm::vec3 pos = { ((float)(i) - (float)(m_Rows)/2.f) * m_Gap + m_CenterX, 0.f, (float)(j + 1) * -m_Gap };
			auto modelTransform = glm::translate(glm::mat4(1.f), pos) * glm::rotate(glm::mat4(1.f), glm::radians(-90.f), { 1.f, 0.f, 0.f });
			for (auto& [id, mesh] : m_Monkey->GetMeshes())
				Apex::ForwardRenderer::SubmitMesh(mesh, m_PhongShader, modelTransform);
		}
		
		
		Apex::ForwardRenderer::EndScene();
	}
	
	void OnEvent(Apex::Event& e) override {}
	
	void OnImGuiRender() override
	{
		ImGui::ShowMetricsWindow();
		
		ImGui::Begin("Controls");
		
		if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
			static float fov = 63.5f;
			if (ImGui::SliderFloat("Field of View", &fov, 0.1f, 90.f))
				m_Camera = Apex::PerspectiveCamera(fov, 16.f / 9.f);
			ImGui::TreePop();
		}
		
		ImGui::SliderInt("Rows", &m_Rows, 1, 100);
		ImGui::DragFloat("Gap", &m_Gap, 0.1f, 1.f, 10.f);
		ImGui::DragFloat("Center X", &m_CenterX, 0.1f, -10.f, 10.f);
		
		for (auto i=0; i<m_ModelFilePaths.size(); i++) {
			if (ImGui::TreeNodeEx(m_ModelFilePaths[i].c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Show", &m_Props[i].Show);
				if (m_Props[i].Show) {
					ImGui::DragFloat3("Position", &m_Props[i].Position.x, 0.1f);
					ImGui::DragFloat("Scale", &m_Props[i].Scale, 1.f, 0.001f, 1000.f, "%.3g", ImGuiSliderFlags_Logarithmic);
					ImGui::gizmo3D("##quat1", m_Props[i].Rotation);
					ImGui::Checkbox("Use Albedo Texture", &m_Props[i].UseAlbedoTexture);
					ImGui::Checkbox("Use Normal Texture", &m_Props[i].UseNormalTexture);
				}
				ImGui::TreePop();
			}
		}
		
		ImGui::Separator();
		
		ImGui::Text("Draw Calls: %ld", Apex::ForwardRenderer::GetNumDrawCalls());
		
		ImGui::End();
	}
	
private:
	Apex::PerspectiveCamera m_Camera;
	Apex::Ref<Apex::Shader> m_PhongShader;
	Apex::Ref<Apex::Shader> m_PhongNormalShader;
	struct ModelProps {
		bool Show = true;
		glm::vec3 Position{ 0.f, 0.f, -5.f };
		float Scale = 1.f;
		glm::quat Rotation{ 1.f, 0.f, 0.f, 0.f };
		bool UseAlbedoTexture = true;
		bool UseNormalTexture = true;
	};
	std::vector<Apex::Ref<Apex::Model>> m_Models;
	std::vector<std::vector<Apex::Ref<Apex::Texture2D>>> m_Textures;
	std::vector<ModelProps> m_Props;
	std::vector<std::string> m_ModelFilePaths = {
// 		"/assets/models/concrete-silo/silo.fbx",
// 		"/assets/models/blender-monkey.fbx",
// 		"/assets/models/revolver/Rheeno.fbx"
	};
	
	Apex::Ref<Apex::Texture2D> m_WhiteTexture;
	
	Apex::Ref<Apex::Model> m_Monkey;
	int m_Rows = 2;
	float m_Gap = 5.f;
	float m_CenterX = 0.f;
};
#endif

#if 0
class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), /*m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraController(&m_Camera)*/
		m_CameraController((float)Apex::Application::Get().GetWindow().GetWidth() / (float)Apex::Application::Get().GetWindow().GetHeight())
	{
		m_Scene = Apex::CreateRef<Apex::Scene>();
	}
	
	~SandboxLayer()
	{
	}
	
	void OnAttach() override
	{
		// Create/Import Resources
		m_BallTexture = Apex::Texture2D::Create("assets/textures/tennis-ball.png");
		m_PusheenTexture = Apex::Texture2D::Create("assets/textures/pusheen-thug-life.png");
		m_NoiseTexture = Apex::Texture2D::Create(256U, 256U, Apex::HDRTextureSpec, "noise");
// 		m_FlatShader = Apex::Shader::Create("assets/shaders/FlatColor.glsl");
		auto computeShader = Apex::ComputeShader::Create("assets/compute/noise.compute");
		
		
		// Execute Compute Shader
		m_NoiseTexture->BindImage(0U, false, true);
		computeShader->Bind();
		computeShader->Dispatch(m_NoiseTexture->GetWidth(), m_NoiseTexture->GetHeight(), 1U);
		
		m_BallEntity = m_Scene->CreateEntity();
		m_BallEntity.AddComponent<Apex::SpriteRendererComponent>(m_BallTexture);
		m_BallEntity.GetComponents<Apex::SpriteRendererComponent>().Color = { 0.78f, 0.54f, 0.23f, 1.f };
		
		Apex::RenderCommands::SetStencilTest(true);
		Apex::RenderCommands::SetDepthTestFunction(Apex::DepthStencilMode::LESS);
	}
	
	void OnDetach() override {}

	void OnUpdate(Apex::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate();
		auto [ballTransformComp, ballSpriteComp] = m_BallEntity.GetComponents<Apex::TransformComponent, Apex::SpriteRendererComponent>();
		ballTransformComp.Transform = glm::translate(glm::mat4(1.f), { m_BallPosition.x, m_BallPosition.y, 0.1f });
		ballSpriteComp.useTexture = true;
		
		m_Rotation += 0.2f * ts;
		m_Rotation = (m_Rotation > 360.f) ? 0.f : m_Rotation;
		
		// Render
		//Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::SetClearColor({ 0.32f, 0.3185f, 0.32f, 1.0f });
		Apex::RenderCommands::Clear();
		
		Apex::RenderCommands::SetDepthTest(true);
// 		Apex::RenderCommands::SetDepthWriteMode(false);
		Apex::RenderCommands::SetStencilWriteMask(0x00);
		Apex::RenderCommands::SetStencilAction(Apex::DepthStencilAction::KEEP, Apex::DepthStencilAction::KEEP, Apex::DepthStencilAction::KEEP);
		
		
		Apex::Renderer2D::BeginScene(m_CameraController.GetCamera());
		
		Apex::Renderer2D::DrawQuad({ 0.f, 0.65f }, { 0.6f, 0.3f }, { 0.85f, 0.45f, 0.67f, 1.f });
		Apex::Renderer2D::DrawQuad({ 0.f, -0.85f }, { 0.9f, 0.2f }, { 0.65f, 0.60f, 0.23f, 1.f });
		
		Apex::Renderer2D::DrawQuad({ -0.4f, 0.f, 0.2f }, { 0.4f, 0.4f }, m_Rotation, m_BallTexture);
		Apex::Renderer2D::DrawQuad({ 0.f, 0.f }, { 0.4f, 0.4f }, m_PusheenTexture);
		Apex::Renderer2D::DrawQuad({ 0.4f, 0.f }, { 0.4f, 0.4f }, m_NoiseTexture, 2.f);
		
		Apex::Renderer2D::EndScene();
		
		Apex::Renderer2D::BeginScene(m_CameraController.GetCamera());
		
		Apex::RenderCommands::SetStencilTestFunction(Apex::DepthStencilMode::ALWAYS, 1);
		Apex::RenderCommands::SetStencilAction(Apex::DepthStencilAction::KEEP, Apex::DepthStencilAction::REPLACE, Apex::DepthStencilAction::REPLACE);
		Apex::RenderCommands::SetStencilWriteMask(0xff);
		m_Scene->Render2D();
		
		Apex::Renderer2D::EndScene();
		
		if (m_ShowOutline) {
			Apex::Renderer2D::BeginScene(m_CameraController.GetCamera());
			
			Apex::RenderCommands::SetStencilTestFunction(Apex::DepthStencilMode::NOTEQUAL, 1);
			Apex::RenderCommands::SetStencilWriteMask(0x00);
			Apex::RenderCommands::SetDepthTest(false);
			
			ballSpriteComp.useTexture = false;
			ballTransformComp.Transform = glm::scale(ballTransformComp.Transform, { 1.04f, 1.04f, 1.f });
			m_Scene->Render2D();
			
			Apex::Renderer2D::EndScene();
			
			Apex::RenderCommands::SetStencilTestFunction(Apex::DepthStencilMode::ALWAYS, 1);
			Apex::RenderCommands::SetStencilAction(Apex::DepthStencilAction::KEEP, Apex::DepthStencilAction::KEEP, Apex::DepthStencilAction::KEEP);
			Apex::RenderCommands::SetStencilWriteMask(0xff);
			Apex::RenderCommands::SetDepthTest(true);
			
			ballTransformComp.Transform = glm::scale(ballTransformComp.Transform, { 1.f / 1.04f, 1.f / 1.04f, 1.f });
		}
	}
	
	void OnEvent(Apex::Event& e)
	{
		m_CameraController.OnEvent(e);
	}
	
	void OnImGuiRender() override
	{
		//static glm::vec2 ballPos = {  };
		
		ImGui::Begin("Entity Controls");
		ImGui::DragFloat2("Position", glm::value_ptr(m_BallPosition), 0.01f);
		ImGui::ColorEdit4("Outline Color", glm::value_ptr(m_BallEntity.GetComponents<Apex::SpriteRendererComponent>().Color));
		ImGui::Checkbox("Show outline", &m_ShowOutline);
		ImGui::End();
		
		/*ImGui::Begin("Textures");
		ImGui::Image((void*)(intptr_t)m_PusheenTexture->GetID(), { 256.f, 256.f });
		ImGui::Image((void*)(intptr_t)m_BallTexture->GetID(), { 256.f, 256.f });
		ImGui::Image((void*)(intptr_t)m_NoiseTexture->GetID(), { 256.f, 256.f });
		ImGui::End();*/
	}
	
private:
	Apex::Ref<Apex::Texture2D> m_PusheenTexture;
	Apex::Ref<Apex::Texture2D> m_NoiseTexture;
	Apex::Ref<Apex::Texture2D> m_BallTexture;
	Apex::Ref<Apex::Shader> m_FlatShader;
	
	Apex::Entity m_BallEntity;
	glm::vec2 m_BallPosition = { 0.f, 0.f };
	
	float m_Rotation = 0.f;
	
	Apex::OrthographicCameraController2D m_CameraController; 
	
	Apex::Ref<Apex::Scene> m_Scene;
	bool m_ShowOutline = false;
};
#endif

class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		//Apex::Log::GetCoreLogger()->set_pattern("%^[%T] <%n> at %@ :: %v%$");
		//Apex::Collision::Init();
		//Apex::NetworkManager::Startup();
		Apex::ForwardRenderer::Init();
		
		PushLayer(new SandboxLayer());
		//PushLayer(new ModelLayer());
		//PushLayer(new ParticleLayer());
		//PushLayer(new DXBall::TestLayer());
		//PushLayer(new TerminalLayer());
	}

	~Sandbox()
	{
		Apex::ForwardRenderer::Shutdown();
		//Apex::NetworkManager::Cleanup();
	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}

#include <Apex/StartUp.h>