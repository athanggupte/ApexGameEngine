#pragma once

#include <fstream>

namespace Apex {

	enum class ProfileCategory
	{
		NONE = 0,
		FUNCTION, SCOPE,
		EVENT,
		OBJECT
	};

	enum class ProfileType
	{
		NONE = 0,
		COMPLETE,

	};

	struct ProfileResult
	{
		std::string name;
		uint64_t start, end;
		uint32_t thread_id;
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
		void WriteInstantEvent(const std::string& name);
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

	protected:
		void Stop();

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
		bool m_Stopped;
	};

}
// #define APEX_PROFILER_ENABLE

#ifndef __FUNCSIG__
#define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

#ifdef APEX_PROFILER_ENABLE
	
	#if defined(APEX_PLATFORM_WINDOWS)
		#define APEX_PROFILE_BEGIN_SESSION(name, filepath)	::Apex::Instrumentor::Get().BeginSession(name, "profiles\\"##filepath)
		#define APEX_PROFILE_END_SESSION()					::Apex::Instrumentor::Get().EndSession()
		#define APEX_PROFILE_SCOPE(name)					::Apex::InstrumentationTimer timer##__LINE__(__FUNCTION__##"::"##name)
		#define APEX_PROFILE_FUNC()							::Apex::InstrumentationTimer timer##__LINE__(__FUNCSIG__)
		#define APEX_PROFILE_EVENT(profile)					::Apex::Instrumentor::Get().WriteInstantEvent(profile)
	#elif defined(APEX_PLATFORM_LINUX)
		#define APEX_PROFILE_BEGIN_SESSION(name, filepath)	::Apex::Instrumentor::Get().BeginSession(name, "profiles/" filepath)
		#define APEX_PROFILE_END_SESSION()					::Apex::Instrumentor::Get().EndSession()
		#define APEX_PROFILE_SCOPE(name)					::Apex::InstrumentationTimer timer##__LINE__(__FUNCTION__ "::" name)
		#define APEX_PROFILE_FUNC()							::Apex::InstrumentationTimer timer##__LINE__(__FUNCSIG__)
		#define APEX_PROFILE_EVENT(profile)					::Apex::Instrumentor::Get().WriteInstantEvent(profile)
	#endif
		
#else
#ifdef _MSC_VER
#pragma message("Profiler is disabled!")
#elif defined(__GNUC__) || defined(__clang__)
#warn Profiler is disabled!
#endif
	#define APEX_PROFILE_BEGIN_SESSION(name, filepath)	
	#define APEX_PROFILE_END_SESSION()					
	#define APEX_PROFILE_SCOPE(name)					
	#define APEX_PROFILE_FUNC()		
	#define APEX_PROFILE_EVENT(profile)
#endif
