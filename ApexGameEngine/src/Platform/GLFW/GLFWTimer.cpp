#include "apex_pch.h"
// #include "GLFWTimer.h"
#include "Apex/Core/Timer.h"

#include <GLFW/glfw3.h>

namespace Apex {

	struct TimerData
	{
		uint64_t timerSteps;
		uint64_t timerFrequency;
		uint64_t frameTimestep;
	};
	
	static TimerData s_TimerData;
	
	void Timer::Init()
	{
		s_TimerData.timerFrequency = glfwGetTimerFrequency();
		s_TimerData.timerSteps = glfwGetTimerValue();
	}

	void Timer::UpdateTime()
	{
		s_TimerData.timerFrequency = glfwGetTimerFrequency();
		const uint64_t curTime = glfwGetTimerValue();
		s_TimerData.frameTimestep = curTime - s_TimerData.timerSteps;
		s_TimerData.timerSteps = curTime;
	}

	Timestep Timer::GetTimestep()
	{
		return { static_cast<float>(s_TimerData.frameTimestep) / static_cast<float>(s_TimerData.timerFrequency) };
	}

	Timestep Timer::GetElapsedTime()
	{
		return { static_cast<float>(s_TimerData.timerSteps) / static_cast<float>(s_TimerData.timerFrequency) };
	}

	uint64_t Timer::GetTimerSteps()
	{
		return s_TimerData.timerSteps;
	}
	
	uint64_t Timer::GetTimerFrequency()
	{
		return s_TimerData.timerFrequency;
	}
	
}
