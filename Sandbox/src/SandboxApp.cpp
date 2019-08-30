#include <Apex.h>

class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox") {}

	void OnUpdate() override
	{
		//APEX_INFO("SandboxLayer::Update()");
	}

	void OnEvent(Apex::Event& event) override
	{
		//APEX_DEBUG("{0}", event);
	}

};


class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
		PushOverlay(new Apex::ImGuiLayer());
	}

	~Sandbox()
	{

	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}