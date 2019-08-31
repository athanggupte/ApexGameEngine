#pragma once

#ifdef APEX_PLATFORM_WINDOWS
	#ifdef APEX_BUILD_DLL
		#define APEX_API __declspec(dllexport)
	#else
		#define APEX_API __declspec(dllimport)
	#endif
#else
	#error Apex only supports Windows for now!
#endif

#ifdef APEX_ENABLE_ASSERTS
	#define APEX_CORE_ASSERT(x, ...) { if(!(x)) { APEX_CORE_ERROR("Assertion Failed : {0}", __VA_ARGS__); __debugbreak(); } }
	#define APEX_ASSERT(x, ...) { if(!(x)) { APEX_ERROR("Assertion Failed : {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define APEX_CORE_ASSERT(x, ...)
	#define APEX_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define APEX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)