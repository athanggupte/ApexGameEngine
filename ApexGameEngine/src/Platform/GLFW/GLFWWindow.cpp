#include "apex_pch.h"
#include "GLFWWindow.h"

#include "Apex/Core/Events/ApplicationEvent.h"
#include "Apex/Core/Events/KeyEvent.h"
#include "Apex/Core/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Apex/Utils/Utils.h" // For ImageData

namespace Apex {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* desc)
	{
		APEX_CORE_ERROR("GLFW Error ({0}) : {1}", error, desc);
	}

	Window * Apex::Window::Create(const WindowProps & props)
	{
		return new GLFWWindow(props);
	}

	GLFWWindow::GLFWWindow(const WindowProps & props)
	{
		Init(props);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}

	void GLFWWindow::Init(const WindowProps & props)
	{
		m_Data.w_Title = props.w_Title;
		m_Data.w_Width = props.w_Width;
		m_Data.w_Height = props.w_Height;

		APEX_CORE_INFO("Creating window \"{0}\" ({1}, {2})", m_Data.w_Title, m_Data.w_Width, m_Data.w_Height);

		if (!s_GLFWInitialized) {
			int success = glfwInit();
			APEX_CORE_ASSERT(success, "Could not initialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		// Set OpenGL debug context
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
		//glfwWindowHint(GLFW_MAXIMIZED, true);
		glfwWindowHint(GLFW_DECORATED, false);
		
		m_Window = glfwCreateWindow((int)m_Data.w_Width, (int)m_Data.w_Height, m_Data.w_Title.c_str(), nullptr, nullptr);
		
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.w_Width = width;
			data.w_Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t character) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			CharTypedEvent event(character);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) 
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double offsetX, double offsetY) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)offsetX, (float)offsetY);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double posX, double posY) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)posX, (float)posY);
			data.EventCallback(event);
		});

	}

	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool GLFWWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	bool GLFWWindow::IsMaximized()
	{
		return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	bool GLFWWindow::IsMinimized()
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	void GLFWWindow::WindowMaximize()
	{
		glfwMaximizeWindow(m_Window);
	}

	void GLFWWindow::WindowRestore()
	{
		glfwRestoreWindow(m_Window);
	}

	void GLFWWindow::WindowMinimize()
	{
		glfwIconifyWindow(m_Window);
	}

	void GLFWWindow::SetWindowIcon(const ImageData& imageData) const
	{
		GLFWimage glfwImage[1] = { { imageData.width, imageData.height, imageData.pixelData->pixels } };
		glfwSetWindowIcon(m_Window, 1, glfwImage);
	}

	void GLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

}
