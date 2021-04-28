#include "apex_pch.h"
#include "Timer.h"

#include "Platform/GLFW/GLFWTimer.h"

namespace Apex {

	Timer * Timer::s_Timer = nullptr;

	void Timer::InitTimer()
	{
		//#ifdef APEX_PLATFORM_WINDOWS
			s_Timer = new GLFWTimer();
		//#else
		//	#error Apex only supports Windows for now!	
		//#endif
	}

}
