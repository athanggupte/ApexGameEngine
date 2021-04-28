#if 0
#pragma once

#include "Apex/Core/Timer.h"

namespace Apex {

	class GLFWTimer : public Timer
	{
	public:
		GLFWTimer();
		~GLFWTimer() {}

	private:
		inline virtual float GetSecondsImpl() override { return static_cast<float>(m_FrameTimeSteps) / static_cast<float>(m_TimerFrequency); }
		inline virtual uint64_t GetTimeStepsImpl() override { return m_TimeSteps; }
		inline virtual uint64_t GetTimerFrequencyImpl() override { return m_TimerFrequency; }
		virtual void UpdateTimeImpl() override;

	private:
		uint64_t m_TimeSteps;
		uint64_t m_TimerFrequency;
		uint64_t m_FrameTimeSteps;
	};

}
#endif
