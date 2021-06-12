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

//#define APEX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define APEX_BIND_EVENT_FN(fn) [this] (auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define _ENABLE_ATOMIC_ALIGNMENT_FIX	// Works only on VS 2015 Update 2 and above

#include "Apex/Core/GUID.h"
#include "Apex/Core/ResourceHandle.hpp"

namespace Apex {

// 	template<typename T>
// 	using Ref = SharedResourceHandle<T>;
// 	
// 	template<typename T, typename... Args>
// 	constexpr typename std::enable_if<std::is_constructible<T, Args&&...>::value, SharedResourceHandle<T>>::type
// 	CreateRef(Args&& ... args)
// 	{
// 		return SharedResourceHandle<T>(new T(std::forward<Args>(args)...));
// 	}

	
// Old Code
	template<typename T>
	using Scope = std::unique_ptr<T>;
	
	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
