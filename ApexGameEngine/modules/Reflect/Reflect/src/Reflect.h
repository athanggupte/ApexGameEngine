#pragma once

#ifdef APEX_REFLECT_TOOL
	#define CLASS(...) class __attribute__((annotate("reflect-class;" #__VA_ARGS__)))
	#define ENUM(...) enum class __attribute__((annotate("reflect-enum;" #__VA_ARGS__)))
	#define PROPERTY(...) __attribute__((annotate("reflect-property;" #__VA_ARGS__)))
	#define METHOD(...) __attribute__((annotate("reflect-method;" #__VA_ARGS__)))
	#define REFLECT_PRIVATE(...)
#else
	#include <Type.h>
	#define CLASS(...) class
	#define ENUM(...) enum class
	#define PROPERTY(...)
	#define METHOD(...)
	#define REFLECT_PRIVATE(name) \
		friend struct ClassImpl<name>;
#endif