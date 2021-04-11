#include "apex_pch.h"
#include "PhysicsTimer.h"

namespace Apex {

	std::chrono::nanoseconds PhysicsTimer::s_DeltaTime = std::chrono::nanoseconds();

	void PhysicsTimer::Init()
	{
		APEX_CORE_TRACE("Initializing PhysicsTimer ...");
		//s_Instance = new PhysicsTimer();
	}

	void PhysicsTimer::SetPhysicsFunction(std::function<void(std::chrono::nanoseconds)> function, uint32_t interval)
	{
		std::thread([function, interval]() {
			std::chrono::time_point prevFrameTime(std::chrono::steady_clock::now());
			while (true)
			{
				auto currFrameTime = std::chrono::steady_clock::now();
				s_DeltaTime = currFrameTime - prevFrameTime;
				prevFrameTime = currFrameTime;
				//APEX_CORE_TRACE("Time Taken: {0} ms", std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());*/
				auto wakeUpTime = currFrameTime/*std::chrono::steady_clock::now()*/ + std::chrono::milliseconds(interval);
				function(s_DeltaTime);
				std::this_thread::sleep_until(wakeUpTime);
			}
		}).detach();
	}

	std::chrono::nanoseconds PhysicsTimer::GetDeltaTime()
	{
		return s_DeltaTime;
	}

}