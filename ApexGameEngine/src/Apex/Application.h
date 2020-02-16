#pragma once

#include "Window.h"
#include "Apex/Layers/LayerStack.h"
#include "Apex/Events/Event.h"
#include "Apex/Events/ApplicationEvent.h"
#include "Apex/Utils/Timer.h"

#include "Apex/ImGui/ImGuiLayer.h"

namespace Apex {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		float m_LastFrameTime;

		static Application* s_Instance;
	};


	//To be defined in client
	Application* CreateApplication();

}
