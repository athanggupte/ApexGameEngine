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

//#include "Apex/Core/ECS/ECSComponent.h"

/****************      Example Layers      *****************/
#include "TestLayer.h"
// #include "ParticleSystemExample.h"
// #include "ModelLoaderExample.h"
// #include "TerminalExample.h"

#if 0
class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
	}
	
	void OnAttach() override
	{
		m_PusheenTexture = Apex::Texture2D::Create("assets/textures/pusheen-thug-life.png");
// 		m_NoiseTexture = Apex::Texture2D_HDR::Create(256U, 256U, "noise");
// 		m_ComputeShader = Apex::ComputeShader::Create("assets/compute/noise.compute");
	}
	
	void OnDetach() override {}

	void OnUpdate() override
	{
		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::Renderer2D::BeginScene(m_Camera);
		
		Apex::Renderer2D::DrawQuad({ 0.5f, 0.65f }, { 0.1f, 0.1f }, m_Color);
		Apex::Renderer2D::DrawQuad({ 0.f, 0.f }, { 1.f, 1.f }, m_PusheenTexture);
		
		Apex::Renderer2D::EndScene();
	}
	
private:
	Apex::Ref<Apex::Texture2D> m_PusheenTexture;
	glm::vec4 m_Color = { 0.85f, 0.45f, 0.67f, 1.f };
	
	Apex::OrthographicCamera m_Camera;
// 	glm::vec3 m_CameraPosition;
// 	float m_CameraRotation = 0.0f;
// 	float m_CameraMoveSpeed = 0.0025f;
// 	float m_CameraRotateSpeed = 30.0f;
};
#endif

class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		Apex::Log::GetCoreLogger()->set_pattern("%^[%T] <%n> at %@ :: %v%$");
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

