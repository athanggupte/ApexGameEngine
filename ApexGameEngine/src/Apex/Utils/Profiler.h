#pragma once

#include <fstream>

namespace Apex {

	struct ProfileResult
	{
		// std::string category;
		std::string name;
		uint64_t start, end;
		uint32_t thread_id;
		// args
	};
	
	struct InstrumentationSession
	{
		std::string name;
	};


	class Instrumentor
	{
	public:
		static Instrumentor& Get();

		void BeginSession(const std::string& name, const std::string& filename = "profile.json");
		void EndSession();

		void WriteHeader();
		void WriteProfile(const ProfileResult& profile);
		void WriteFooter();
	
	private:
		Instrumentor();

	private:
		static Instrumentor* s_Instance;

		InstrumentationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
		int m_ProfileCount;
	};


	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char* name);
		~InstrumentationTimer();

#define PROFILE_FUNC Apex::InstrumentationTimer timer##__LINE__(__FUNCSIG__)
#define PROFILE_SCOPE(x) Apex::InstrumentationTimer timer##__LINE__(x)

	protected:
		void Stop();

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
		bool m_Stopped;
	};

}