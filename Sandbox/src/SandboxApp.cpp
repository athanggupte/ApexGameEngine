#include <Apex.h>

#include <imgui.h>

class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox") {}

	void OnUpdate() override
	{

	}

	void OnEvent(Apex::Event& event) override
	{

	}

	void OnImGuiRender() override
	{
		ImGui::ShowDemoWindow();
	}

};


class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{

	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}