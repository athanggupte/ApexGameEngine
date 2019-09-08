#include "apex_pch.h"
#include "WindowsTimer.h"

#include <GLFW/glfw3.h>

namespace Apex {

	WindowsTimer::WindowsTimer()
	{
		m_TimerFrequency = glfwGetTimerFrequency();
		m_TimeSteps = glfwGetTimerValue();
	}

	void WindowsTimer::UpdateTimeImpl()
	{
		m_TimerFrequency = glfwGetTimerFrequency();
		uint64_t curTime = glfwGetTimerValue();
		m_FrameTimeSteps = curTime - m_TimeSteps;
		m_TimeSteps = curTime;
	}

}