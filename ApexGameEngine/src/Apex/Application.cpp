#include "apex_pch.h"
#include "Application.h"

#include "Apex/Events/ApplicationEvent.h"
#include "Apex/Log/Log.h"

namespace Apex {

#define BIND_CALLBACK_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_CALLBACK_FN(OnEvent));
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running) {
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event & e)
	{
		APEX_CORE_DEBUG("{0}", e);
	}

}