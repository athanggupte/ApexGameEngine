#include "apex_pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Apex {

	OpenGLContext::OpenGLContext(GLFWwindow * windowHandle)
		: m_WindowHandle(windowHandle)
	{
	}

	static void APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, uint32_t id, GLenum severity,
											 GLsizei length, const char* message, const void* userParam)
	{
		bool errorFlag = false;
		std::stringstream ss;
		ss << "ID : <" << id << ">: " << message << '\n';

		ss << "\t\tSource : ";
		switch (source) {
			case GL_DEBUG_SOURCE_API:				ss << "OpenGL API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		ss << "Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:	ss << "Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:		ss << "Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION:		ss << "User Application"; break;
			case GL_DEBUG_SOURCE_OTHER:				ss << "Other"; break;
		}
		ss << '\n';

		ss << "\t\tType : ";
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:				ss << "Error"; errorFlag = true; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	ss << "Deprecated Behaviour"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	ss << "Undefined Behaviour"; break; 
			case GL_DEBUG_TYPE_PORTABILITY:			ss << "Portability"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:			ss << "Performance"; break;
			case GL_DEBUG_TYPE_MARKER:				ss << "Marker"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:			ss << "Push Group"; break;
			case GL_DEBUG_TYPE_POP_GROUP:			ss << "Pop Group"; break;
			case GL_DEBUG_TYPE_OTHER:				ss << "Other"; break;
		}
		ss << '\n';

		ss << "\t\tSeverity : ";
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH:			ss << "HIGH"; if (errorFlag) { APEX_CORE_CRITICAL("{0}", ss.str()); } else APEX_CORE_ERROR("{0}", ss.str()); break;
			case GL_DEBUG_SEVERITY_MEDIUM:			ss << "MEDIUM"; if (errorFlag) APEX_CORE_ERROR("{0}", ss.str()); else APEX_CORE_WARN("{0}", ss.str()); break;
			case GL_DEBUG_SEVERITY_LOW:				ss << "LOW"; APEX_CORE_WARN("{0}", ss.str()); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:	ss << "NOTIF"; APEX_CORE_TRACE("{0}", ss.str()); break;
		}
	}
	
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);

		// Initialize Glad
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		APEX_CORE_ASSERT(status, "Could not initialize Glad!");

		APEX_CORE_DEBUG("OpenGL Details :\n\tVendor\t\t: {0}\n\tRenderer\t: {1}\n\tVersion\t\t: {2}",
			glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));
		
#ifdef APEX_DEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
		glDebugMessageCallback(OpenGLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		//glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
