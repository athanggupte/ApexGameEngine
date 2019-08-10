#pragma once

#include "Core.h"

#include "Window.h"
#include "Apex/Layers/LayerStack.h"
#include "Apex/Events/Event.h"
#include "Apex/Events/ApplicationEvent.h"

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

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};


	//To be defined in client
	Application* CreateApplication();

}
