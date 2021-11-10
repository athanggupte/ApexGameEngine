#pragma once

#include "Window.h"
#include "Apex/Core/Layers/LayerStack.h"
#include "Apex/Core/Events/Event.h"
#include "Apex/Core/Events/ApplicationEvent.h"
#include "Apex/Core/Timer.h"

#include "Apex/ImGui/ImGuiLayer.h"

namespace Apex {

	class ResourceManager;

	class Application
	{
	public:
		Application(const WindowProps& windowProps = WindowProps());
		virtual ~Application();

		void Run();
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline ResourceManager& GetResourceManager() { return *m_ResourceManager; }
		inline ImGuiLayer& GetImGuiLayer() { return *m_ImGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Unique<Window> m_Window;
		Unique<ResourceManager> m_ResourceManager;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime;

		static Application* s_Instance;
	};


	//To be defined in client
	Application* CreateApplication();

}
