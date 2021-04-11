#pragma once

namespace Apex {

	class Timer
	{
	public:
		static uint64_t GetTimerSteps() { return s_Timer->GetTimeStepsImpl(); }
		static uint64_t GetTimerFrequency() { return s_Timer->GetTimerFrequencyImpl(); }

		static void UpdateTime() { s_Timer->UpdateTimeImpl(); }

		static float GetSeconds() { return s_Timer->GetSecondsImpl(); }
		static float GetMillis() { return s_Timer->GetSecondsImpl() * 1000.0f; }

		static void InitTimer();

	protected:
		virtual float GetSecondsImpl() = 0;
		virtual uint64_t GetTimeStepsImpl() = 0;
		virtual uint64_t GetTimerFrequencyImpl() = 0;
		virtual void UpdateTimeImpl() = 0;

	private:
		static Timer* s_Timer;

	};

}