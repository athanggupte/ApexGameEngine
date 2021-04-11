#include <Apex.h>
#include <imgui.h>

// #include <future>
// #include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "Apex/Graphics/Material/Material.h"
//#include "Apex/Graphics/Model/Model.h"
//#include "Apex/Utils/ComputeShader/ComputeShader.h"
//#include "Apex/Physics/ParticleSystem/ParticleSystem2D.h"

//#include "Games/DXBall/DXBall.h"

// Networking
//#include "Apex/Networking/NetworkManager.h"

// Audio
//#include "irrKlang.h"

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Components.h"
#include "Apex/Core/CameraController.h"

/****************      Example Layers      *****************/
// #include "TestLayer.h"
// #include "ParticleSystemExample.h"
// #include "ModelLoaderExample.h"
// #include "TerminalExample.h"

// #if 0
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

	void OnUpdate() override
	{
		// Update
		m_CameraController.OnUpdate();
		auto [ballTransformComp, ballSpriteComp] = m_BallEntity.GetComponents<Apex::TransformComponent, Apex::SpriteRendererComponent>();
		ballTransformComp.Transform = glm::translate(glm::mat4(1.f), { m_BallPosition.x, m_BallPosition.y, 0.1f });
		ballSpriteComp.useTexture = true;
		
		m_Rotation += 0.2f * Apex::Timer::GetSeconds();
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
// #endif

class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		//Apex::Log::GetCoreLogger()->set_pattern("%^[%T] <%n> at %@ :: %v%$");
		//Apex::Collision::Init();
		//Apex::NetworkManager::Startup();

		PushLayer(new SandboxLayer());
		//PushLayer(new ModelLayer());
		//PushLayer(new ParticleLayer());
		//PushLayer(new DXBall::TestLayer());
		//PushLayer(new TerminalLayer());
	}

	~Sandbox()
	{
		//Apex::NetworkManager::Cleanup();
	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}

