#pragma once

#ifdef REFLECT_DLL
	#ifdef APEX_PLATFORM_WINDOWS
		#define REFLECT_DLL_EXPORT _declspec(dllexport)
		#define REFLECT_DLL_IMPORT _declspec(dllimport)
	#else
		#define REFLECT_DLL_EXPORT
		#define REFLECT_DLL_IMPORT
	#endif


	#ifdef REFLECT_EXPORTS
		#define REFLECT_API REFLECT_DLL_EXPORT
	#else
		#define REFLECT_API REFLECT_DLL_IMPORT
	#endif
#endif


#ifndef REFLECT_API
	#define REFLECT_API
#endif

#ifdef APEX_CORE_DEBUG
	#define LOG_DEBUG(...) APEX_CORE_DEBUG(__VA_ARGS__)
#endif