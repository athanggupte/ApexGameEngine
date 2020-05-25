#include <apex_pch.h>
#include "Profiler.h"

namespace Apex {

	//////////////// Instrumentator ///////////////

	Instrumentor* Instrumentor::s_Instance = nullptr;
	
	Instrumentor::Instrumentor()
		: m_CurrentSession(nullptr), m_ProfileCount(0)
	{
	}

	Instrumentor& Apex::Instrumentor::Get()
	{
		if (!s_Instance)
			s_Instance = new Instrumentor();
		return *s_Instance;
	}

	void Instrumentor::BeginSession(const std::string& name, const std::string& filename)
	{
		m_OutputStream.open(filename);
		WriteHeader();
		m_CurrentSession = new InstrumentationSession{ name };
	}

	void Instrumentor::EndSession()
	{
		WriteFooter();
		m_OutputStream.close();
		delete m_CurrentSession;
		m_CurrentSession = nullptr;
		m_ProfileCount = 0;
	}
	
	void Instrumentor::WriteHeader()
	{
		m_OutputStream << "{\"otherData\": {}, \"traceEvents\":[";
		m_OutputStream.flush();
	}

	void Instrumentor::WriteProfile(const ProfileResult& profile)
	{
		if (m_ProfileCount++ > 0)
			m_OutputStream << ",";

		std::string name = profile.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_OutputStream << "{";
		m_OutputStream << "\"cat\":\"function\",";
		m_OutputStream << "\"dur\":" << (profile.end - profile.start) << ",";
		m_OutputStream << "\"name\":\"" << name << "\",";
		m_OutputStream << "\"ph\":\"X\",";
		m_OutputStream << "\"pid\":0,";
		m_OutputStream << "\"tid\":" << profile.thread_id << ",";
		m_OutputStream << "\"ts\":" << profile.start;
		m_OutputStream << "}";

		m_OutputStream.flush();
	}

	void Instrumentor::WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}
	

	////////////// InstrumentationTimer /////////////////

	InstrumentationTimer::InstrumentationTimer(const char* name)
		: m_Name(name), m_Stopped(false)
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	InstrumentationTimer::~InstrumentationTimer()
	{
		if (!m_Stopped)
			Stop();
	}

	void InstrumentationTimer::Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
		uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		uint32_t thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());

		Instrumentor::Get().WriteProfile({ m_Name, start, end, thread_id });
		
		
		m_Stopped = true;
	}

}
