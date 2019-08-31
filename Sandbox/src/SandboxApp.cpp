#include <Apex.h>

#include <imgui.h>

class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox") {}

	void OnUpdate() override
	{
		if (Apex::Input::IsKeyPressed(APEX_KEY_SPACE))
			APEX_TRACE("__SPACE__ (poll)");
	}

	void OnEvent(Apex::Event& event) override
	{
		if (event.GetEventType() == Apex::EventType::KeyPressed) {
			Apex::KeyPressedEvent& e = (Apex::KeyPressedEvent&)event;
			if (e.GetKeyCode() == APEX_KEY_SPACE)
				APEX_TRACE("__SPACE__ (event)");
		}
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Button("Test Button", { 20.f, 20.f });
		ImGui::End();
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