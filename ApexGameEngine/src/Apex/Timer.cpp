#include "apex_pch.h"
#include "Timer.h"

#include "Platform/Windows/WindowsTimer.h"

namespace Apex {

	Timer * Timer::s_Timer = nullptr;

	void Timer::InitTimer()
	{
		#ifdef APEX_PLATFORM_WINDOWS
			s_Timer = new WindowsTimer();
		#else
			#error Apex only supports Windows for now!	
		#endif
	}

}