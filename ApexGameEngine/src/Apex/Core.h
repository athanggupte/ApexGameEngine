#pragma once

#include <memory>

#ifdef APEX_PLATFORM_WINDOWS
	#ifdef APEX_DYNAMIC_LINK
		#ifdef APEX_BUILD_DLL
			#define _APEX_API __declspec(dllexport)
		#else
			#define _APEX_API __declspec(dllimport)
		#endif
	#else
		#define _APEX_API
	#endif
	// Define DebugBreak
    #define __DEBUG_BREAK__ __debugbreak()
	
#elif defined APEX_PLATFORM_LINUX
	#define _APEX_API
	#if defined(__i386__) || defined(__x86_64__)
		static inline void debug_trap(void) { __asm__ __volatile__("int3"); }
		#define __DEBUG_BREAK__ debug_trap()
	#endif
#else
	#define _APEX_API
	#define __DEBUG_BREAK__
#endif

#ifdef APEX_ENABLE_ASSERTS
	#define APEX_CORE_ASSERT(x, ...) { if(!(x)) { APEX_CORE_ERROR("Assertion Failed : {0}", __VA_ARGS__); __DEBUG_BREAK__; } }
	#define APEX_ASSERT(x, ...) { if(!(x)) { APEX_LOG_ERROR("Assertion Failed : {0}", __VA_ARGS__); __DEBUG_BREAK__; } }
#else
	#define APEX_CORE_ASSERT(x, ...)
	#define APEX_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define APEX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define _ENABLE_ATOMIC_ALIGNMENT_FIX	// Works only on VS 2015 Update 2 and above

namespace Apex {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}
