#pragma once

namespace Apex {

	class APEX_API Timestep
	{
	public:
		Timestep(float time = 0.f) : m_Time(time) {}
		
		operator float () const { return m_Time; }

		[[nodiscard]] float GetSeconds() const { return m_Time; }
		[[nodiscard]] float GetMillis() const { return m_Time * 1000.0f; }


		Timestep& operator += (const Timestep& ts) { this->m_Time += ts.m_Time; return *this; }
		Timestep& operator -= (const Timestep& ts) { this->m_Time -= ts.m_Time; return *this; }

		const Timestep operator + (const Timestep& ts) const { return Timestep{*this} += ts; }
		const Timestep operator - (const Timestep& ts) const { return Timestep{*this} -= ts; }

	private:
		float m_Time;
	};

	
	class APEX_API Timer
	{
	public:
		static void Init();
		static void UpdateTime();
		
		[[nodiscard]] static uint64_t GetTimerSteps();
		[[nodiscard]] static uint64_t GetTimerFrequency();
		
		[[nodiscard]] static Timestep GetTimestep();
		[[nodiscard]] static Timestep GetElapsedTime();
	};
	
}
