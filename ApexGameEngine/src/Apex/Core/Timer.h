#pragma once

namespace Apex {

	class Timestep
	{
	public:
		Timestep(float time = 0.f) : m_Time(time) {}
		
		operator float () const { return m_Time; }
		
		float GetSeconds() { return m_Time; }
		float GetMillis() { return m_Time * 1000.0f; }
		
	private:
		float m_Time;
	};

	
	class Timer
	{
	public:
		static void Init();
		static void UpdateTime();
		
		static uint64_t GetTimerSteps();
		static uint64_t GetTimerFrequency();
		
		static Timestep GetTimestep();
	};
	
}
