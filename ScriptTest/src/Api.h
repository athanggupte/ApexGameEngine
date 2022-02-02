#pragma once

#ifdef SCRIPT_TEST_EXPORTS
	#define SCRIPT_TEST_API __declspec(dllexport)
#else
	#define SCRIPT_TEST_API __declspec(dllimport)
#endif
