#include <Apex.h>

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