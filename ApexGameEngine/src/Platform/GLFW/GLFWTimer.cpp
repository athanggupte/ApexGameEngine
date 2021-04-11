#include "apex_pch.h"
#include "GLFWTimer.h"

#include <GLFW/glfw3.h>

namespace Apex {

	GLFWTimer::GLFWTimer()
	{
		m_TimerFrequency = glfwGetTimerFrequency();
		m_TimeSteps = glfwGetTimerValue();
	}

	void GLFWTimer::UpdateTimeImpl()
	{
		m_TimerFrequency = glfwGetTimerFrequency();
		uint64_t curTime = glfwGetTimerValue();
		m_FrameTimeSteps = curTime - m_TimeSteps;
		m_TimeSteps = curTime;
	}

}
