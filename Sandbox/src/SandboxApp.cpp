#include <Apex.h>
#include <imgui.h>

// #include <future>
// #include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

//#include "Apex/Graphics/Material/Material.h"
//#include "Apex/Graphics/Model/Model.h"
//#include "Apex/Utils/ComputeShader/ComputeShader.h"
//#include "Apex/Physics/ParticleSystem/ParticleSystem2D.h"

//#include "Games/DXBall/DXBall.h"

// Networking
//#include "Apex/Networking/NetworkManager.h"

// Physics
#include "Apex/Physics/Particle.h"
#include "Apex/Physics/PhysicsSystem.h"

// Audio
//#include "irrKlang.h"

/****************      Example Layers      *****************/
// #include "TestLayer.h"
// #include "ObjectOutlineTest.h"
// #include "ParticleSystemExample.h"
// #include "ModelLoaderExample.h"
// #include "TerminalExample.h"

// #if 0
class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), /*m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraController(&m_Camera)*/
		m_CameraController((float)Apex::Application::Get().GetWindow().GetWidth() / (float)Apex::Application::Get().GetWindow().GetHeight()),
// 		m_P1(), m_P2(), m_P3(),
		m_SpringForceP1(15.0, 0.5), m_SpringForceP2(15.0, 0.5), m_AnchoredSpring(15.0, 0.5),
		m_AnchoredCable(1.0, 0.5)
	{
		m_Scene = Apex::CreateRef<Apex::Scene>();
// 		m_PFRegistry = Apex::CreateRef<Apex::Physics::ParticleForceRegistry>();
		Apex::PhysicsSystem::SetScene(m_Scene.Get());
		
		m_E1 = m_Scene->CreateEntity("Ball-1");
		m_E2 = m_Scene->CreateEntity("Ball-2");
	}
	
	~SandboxLayer()
	{
	}
	
	void OnAttach() override
	{
		// Create/Import Resources
		m_BallTexture = Apex::Texture2D::Create("assets/textures/tennis-ball.png");
		
// 		m_BallEntity = m_Scene->CreateEntity();
// 		m_BallEntity.AddComponent<Apex::SpriteRendererComponent>(m_BallTexture);
// 		m_BallEntity.GetComponents<Apex::SpriteRendererComponent>().Color = { 0.78f, 0.54f, 0.23f, 1.f };
		
		m_E1.AddComponent<Apex::SpriteRendererComponent>(m_BallTexture);
		m_E2.AddComponent<Apex::SpriteRendererComponent>(m_BallTexture);
		
		Apex::Physics::Particle dummyParticle;
		dummyParticle.Position = { -0.5, 0.7, 0.0 };
		dummyParticle.Acceleration = { 0.0, -1.0, 0.0 };
		dummyParticle.Damping = 0.85;
		dummyParticle.SetMass(10.0);
		
		m_E1.AddComponent<Apex::ParticleComponent>(dummyParticle);
		auto& e1comp = m_E1.GetComponents<Apex::ParticleComponent>();
// 		m_SpringForceP1.SetOtherParticle(e1comp.ParticlePtr);
		m_AnchoredCable.SetParticle(e1comp.ParticlePtr);
		
		dummyParticle.Acceleration = { 0.0, -1.0, 0.0 };
		dummyParticle.Position = { 0.7, 0.5, 0.0 };
		
// 		m_E2.AddComponent<Apex::ParticleComponent>(dummyParticle);
// 		auto& e2comp = m_E2.GetComponents<Apex::ParticleComponent>();
// 		m_SpringForceP2.SetOtherParticle(e2comp.ParticlePtr);
		
		m_Anchor = { 0.0, 0.7, 0.0 };
// 		m_AnchoredSpring.SetAnchor(&m_Anchor);
		m_AnchoredCable.SetAnchor(&m_Anchor);
		
// 		Apex::PhysicsSystem::AttachForceGenerator(m_E1, &m_SpringForceP2);
// 		Apex::PhysicsSystem::AttachForceGenerator(m_E2, &m_SpringForceP1);
// 		
// 		Apex::PhysicsSystem::AttachForceGenerator(m_E2, &m_AnchoredSpring);
		Apex::PhysicsSystem::AddContactGenerator(&m_AnchoredCable);
		
	}
	
	void OnDetach() override
	{
	}

	void OnUpdate() override
	{
		// Input Update
		m_CameraController.OnUpdate();
// 		auto [ballTransformComp, ballSpriteComp] = m_BallEntity.GetComponents<Apex::TransformComponent, Apex::SpriteRendererComponent>();
// 		ballTransformComp.Transform = glm::translate(glm::mat4(1.f), { m_BallPosition.x, m_BallPosition.y, 0.1f });
		
		Apex::PhysicsSystem::StartFrame();
		
		auto& trfE1 = m_E1.GetComponents<Apex::TransformComponent>();
		trfE1 = glm::scale(glm::mat4(1.f), { 0.05f, 0.05f, 1.f });
		
		auto& trfE2 = m_E2.GetComponents<Apex::TransformComponent>();
		trfE2 = glm::scale(glm::mat4(1.f), { 0.05f, 0.05f, 1.f });
		
		// Render
		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		
		Apex::Renderer2D::BeginScene(m_CameraController.GetCamera());
		
// 		Apex::Renderer2D::DrawQuad(m_P1.Position, { 0.05f, 0.05f }, m_BallTexture);
// 		Apex::Renderer2D::DrawQuad(m_P2.Position, { 0.05f, 0.05f }, m_BallTexture);
		
		m_Scene->OnUpdate(false);
		m_Scene->Render2D();
		
		Apex::Renderer2D::EndScene();
		
		// Physics Update
		auto dt = Apex::Timer::GetSeconds();
		Apex::PhysicsSystem::Update(dt);
		
		// Logging
	}
	
	void OnEvent(Apex::Event& e)
	{
		m_CameraController.OnEvent(e);
	}
	
	void OnImGuiRender() override
	{
		auto p1 = m_E1.GetComponents<Apex::ParticleComponent>().ParticlePtr;
// 		auto p2 = m_E2.GetComponents<Apex::ParticleComponent>().ParticlePtr;
		
		ImGui::Begin("Logging");
		ImGui::Text("Used Contacts: %d", Apex::PhysicsSystem::GetNumContactsInFrame());
		ImGui::Text("P1 :: pos : %f, %f, %f", p1->Position.x, p1->Position.y, p1->Position.z);
// 		ImGui::Text("P2 :: pos : %f, %f, %f", p2->Position.x, p2->Position.y, p2->Position.z);
		ImGui::End();
	}
	
private:
	Apex::Ref<Apex::Scene> m_Scene;
	Apex::Entity m_E1, m_E2;
	
// 	Apex::Ref<Apex::Physics::ParticleForceRegistry> m_PFRegistry;
// 	Apex::Physics::Particle m_P1, m_P2, m_P3;
	Apex::Physics::SpringPFG m_SpringForceP1, m_SpringForceP2;
	Apex::Physics::AnchoredSpringPFG m_AnchoredSpring;
	Apex::Physics::ParticleAnchoredCable m_AnchoredCable;
	glm::vec3 m_Anchor;
	//Apex::Physics::DragPFG m_AirDrag;
	
	Apex::Ref<Apex::Texture2D> m_BallTexture;
	
	Apex::OrthographicCameraController2D m_CameraController; 
	
	
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

