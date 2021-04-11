#pragma once

namespace Apex {
	
	class PhysicsTimer
	{
	public:
		static void Init();
		static void SetPhysicsFunction(std::function<void(std::chrono::nanoseconds)> function, uint32_t interval);

		static std::chrono::nanoseconds GetDeltaTime();
	private:
		static std::chrono::nanoseconds s_DeltaTime;
		//static std::vector<std::thread> s_Threads;
	};

}