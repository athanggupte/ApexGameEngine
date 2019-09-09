#pragma once

#include "Apex/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Apex {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};

}

//Core Log Macros
#define APEX_CORE_TRACE(...)		::Apex::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define APEX_CORE_DEBUG(...)		::Apex::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define APEX_CORE_INFO(...)			::Apex::Log::GetCoreLogger()->info(__VA_ARGS__)
#define APEX_CORE_WARN(...)			::Apex::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define APEX_CORE_ERROR(...)		::Apex::Log::GetCoreLogger()->error(__VA_ARGS__)
#define APEX_CORE_CRITICAL(...)		::Apex::Log::GetCoreLogger()->critical(__VA_ARGS__); __debugbreak();

//Client Log Macros
#define APEX_TRACE(...)				::Apex::Log::GetClientLogger()->trace(__VA_ARGS__)
#define APEX_INFO(...)				::Apex::Log::GetClientLogger()->info(__VA_ARGS__)
#define APEX_WARN(...)				::Apex::Log::GetClientLogger()->warn(__VA_ARGS__)
#define APEX_ERROR(...)				::Apex::Log::GetClientLogger()->error(__VA_ARGS__)
#define APEX_CRITICAL(...)			::Apex::Log::GetClientLogger()->critical(__VA_ARGS__); __debugbreak();