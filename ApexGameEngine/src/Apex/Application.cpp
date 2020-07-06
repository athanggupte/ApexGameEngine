#include "apex_pch.h"
#include "Application.h"

#include "Apex/Core/Log.h"
#include "Apex/Core/Input/Input.h"
#include "Apex/Graphics/Renderer/Renderer.h"

//#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Apex {

#define BIND_CALLBACK_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		APEX_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window = Apex::Scope<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_CALLBACK_FN(OnEvent));
		m_Window->SetVSync(true);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

	}


	Application::~Application()
	{
	}

	void Application::Run()
	{

		while (m_Running) {

			Timer::UpdateTime();

			if (!m_Minimized) {
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate();
			}

			//Render ImGui
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			//if (!m_Minimized) {
			m_Window->OnUpdate();
			//}
			GLenum error = glGetError();
			if (error != GL_NO_ERROR) APEX_CORE_ERROR("[OpenGL] :: {0}", error);
		}

	}

	void Application::Close()
	{
		m_Running = false;
	}

	// Event Handlers

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_CALLBACK_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_CALLBACK_FN(OnWindowResize));

#if 0
		if (e.GetType() == EventType::WindowCloseEvent)
		{
			this->OnWindowClose(e);
		}
#endif

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_Running = false;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		return false;
	}

}