#include "apex_pch.h"
#include "WindowsWindow.h"

namespace Apex {

	static bool s_GLFWInitialized = false;

	Window * Apex::Window::Create(const WindowProps & props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps & props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps & props)
	{
		m_Data.w_Title = props.w_Title;
		m_Data.w_Width = props.w_Width;
		m_Data.w_Height = props.w_Height;

		APEX_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.w_Title, m_Data.w_Width, m_Data.w_Height);

		if (!s_GLFWInitialized) {
			int success = glfwInit();
			APEX_CORE_ASSERT(success, "Could not initialize GLFW!");

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)m_Data.w_Width, (int)m_Data.w_Height, m_Data.w_Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks

	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

}