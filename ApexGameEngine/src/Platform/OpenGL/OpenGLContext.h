#pragma once

#include "Apex/Renderer/RenderingContext.h"

struct GLFWwindow;

namespace Apex {

	class OpenGLContext : public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};

}