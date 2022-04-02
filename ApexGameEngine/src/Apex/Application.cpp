#include "apex_pch.h"
#include "Application.h"

#include "Apex/Core/Log.h"
#include "Apex/Core/Input/Input.h"
#include "Apex/Core/FileSystem/FileSystem.h"
#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/Renderer/Renderer.h"
#include "Apex/Graphics/Renderer/Renderer2D.h"
#include "Apex/Graphics/PostProcessing/PostProcess.h"
#include "Graphics/Renderer/TextRenderer.h"

//#include <GLFW/glfw3.h>
//#include <glad/glad.h>

namespace Apex {

#define BIND_CALLBACK_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& windowProps)
	{
		APEX_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window = Apex::Unique<Window>(Window::Create(windowProps));
		m_Window->SetEventCallback(BIND_CALLBACK_FN(OnEvent));
		m_Window->SetVSync(true);

		m_ResourceManager = Apex::CreateUnique<ResourceManager>();

		FileSystem::Init();
		Renderer::Init();
		Renderer2D::Init();
		TextRenderer::Init();
		PostProcess::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}


	Application::~Application()
	{
		Renderer::Shutdown();
		Renderer2D::Shutdown();
		TextRenderer::Shutdown();
		PostProcess::Shutdown();
		FileSystem::Shutdown();
	}

	void Application::Run()
	{

		while (m_Running) {

			Timer::UpdateTime();
			auto timestep = Timer::GetTimestep();

			if (!m_Minimized) {
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			//Render ImGui
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			//if (!m_Minimized) {
			m_Window->OnUpdate();
			//}
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

		RenderCommands::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		
		return false;
	}

}
