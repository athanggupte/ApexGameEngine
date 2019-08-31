#pragma once

#include "Core.h"

#include "Window.h"
#include "Apex/Layers/LayerStack.h"
#include "Apex/Events/Event.h"
#include "Apex/Events/ApplicationEvent.h"

#include "Apex/ImGui/ImGuiLayer.h"

namespace Apex {

	class APEX_API Application
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

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		static Application* s_Instance;
	};


	//To be defined in client
	Application* CreateApplication();

}
