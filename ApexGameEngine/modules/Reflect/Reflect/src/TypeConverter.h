#pragma once

#include "Any.h"
#include "TypeImpl.hpp"
#include "ArithmeticConverter.h"

namespace detail {

	// Inspired by https://github.dev/rttrorg/rttr/blob/master/src/rttr/detail/variant/variant_data_converter.h

	template <typename T>
	struct ConvertFrom;

	template <typename T, typename TypeConverter = ConvertFrom<T>>
	struct DefaultTypeConverter
	{
		static bool ConvertTo(const T& value, Any& to)
		{
			Type* targetType = to.Type();

			if (targetType == GetType<bool>())
				return TypeConverter::To(value, to.Cast<bool>());
			if (targetType == GetType<char>())
				return TypeConverter::To(value, to.Cast<char>());
			//if (targetType == GetType<int8_t>())
			//	return TypeConverter::To(value, to.Cast<int8_t>());
			if (targetType == GetType<short>())
				return TypeConverter::To(value, to.Cast<short>());
			if (targetType == GetType<int>())
				return TypeConverter::To(value, to.Cast<int>());
			if (targetType == GetType<long long>())
				return TypeConverter::To(value, to.Cast<long long>());
			if (targetType == GetType<unsigned char>())
				return TypeConverter::To(value, to.Cast<unsigned char>());
			if (targetType == GetType<unsigned short>())
				return TypeConverter::To(value, to.Cast<unsigned short>());
			if (targetType == GetType<unsigned int>())
				return TypeConverter::To(value, to.Cast<unsigned int>());
			if (targetType == GetType<unsigned long long>())
				return TypeConverter::To(value, to.Cast<unsigned long long>());
			if (targetType == GetType<float>())
				return TypeConverter::To(value, to.Cast<float>());
			if (targetType == GetType<double>())
				return TypeConverter::To(value, to.Cast<double>());
			if (targetType == GetType<std::string>())
				return TypeConverter::To(value, to.Cast<std::string>());
			//if (targetType == GetType<std::wstring>())
			//	return TypeConverter::To(value, to.Cast<std::wstring>());
			return false;
		}
	};

	template <typename F>
	struct ConvertFrom
	{
		template <typename T>
		static inline bool To(const F&, T&)
		{
			return false;
		}
	};

	////// bool //////
	template <>
	struct ConvertFrom<bool>
	{
		static inline bool To(const bool& from, bool& to)
		{
			to = from;
			return true;
		}

		static inline bool To(const bool& from, char& to)
		{
			to = static_cast<char>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, short& to)
		{
			to = static_cast<short>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, int& to)
		{
			to = static_cast<int>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, long long& to)
		{
			to = static_cast<long long>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, unsigned char& to)
		{
			to = static_cast<unsigned char>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, unsigned short& to)
		{
			to = static_cast<unsigned short>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, unsigned int& to)
		{
			to = static_cast<unsigned int>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, unsigned long long& to)
		{
			to = static_cast<unsigned long long>(from ? 1 : 0);
			return true;
		}

		static inline bool To(const bool& from, float& to)
		{
			to = static_cast<float>(from ? 1.f : 0.f);
			return true;
		}

		static inline bool To(const bool& from, double& to)
		{
			to = static_cast<double>(from ? 1.0 : 0.0);
			return true;
		}

		static inline bool To(const bool& from, std::string& to)
		{
			to = (from ? "true" : "false");
			return true;
		}
	};

	////// char //////
	template <>
	struct ConvertFrom<char>
	{
		template <typename T>
		static inline 
		std::enable_if_t<std::is_integral_v<T>, bool>
		To(const char& from, T& to)
		{
			return Convert(from, to);
		}

		template <typename T>
		static inline 
		std::enable_if_t<std::is_floating_point_v<T>, bool>
		To(const char& from, T& to)
		{
			to = static_cast<T>(from);
			return true;
		}

		static inline bool To(const char& from, std::string& to)
		{
			to = std::string(1, from);
			return true;
		}
	};

	////// int //////
	template <>
	struct ConvertFrom<int>
	{
		template <typename T>
		static inline 
		std::enable_if_t<std::is_integral_v<T>, bool>
		To(const int& from, T& to)
		{
			return Convert(from, to);
		}

		template <typename T>
		static inline 
		std::enable_if_t<std::is_floating_point_v<T>, bool>
		To(const int& from, T& to)
		{
			to = static_cast<T>(from);
			return true;
		}

		static inline bool To(const int& from, std::string& to)
		{
			to = std::to_string(from);
			return true;
		}
	};
}