#pragma once

#ifdef APEX_PLATFORM_WINDOWS
	#ifdef APEX_BUILD_DLL
		#define APEX_API __declspec(dllexport)
	#else
		#define APEX_API __declspec(dllimport)
	#endif
#else
	#error Apex only support Windows for now!
#endif