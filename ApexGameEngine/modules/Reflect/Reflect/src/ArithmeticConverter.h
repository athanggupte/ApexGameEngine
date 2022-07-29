#pragma once
#include "TypeTraits.h"
#include <limits>

namespace detail {

	// Same type
	template <typename F, typename T>
	inline
	std::enable_if_t<std::is_same_v<F, T>, bool>
	Convert(const F& from, T& to)
	{
		to = from;
		return true;
	}

	// Integer - boolean
	template <typename F>
	inline
	std::enable_if_t<is_integral_not_bool<F>::value, bool>
	Convert(const F& from, bool& to)
	{
		to = (from != 0);
		return true;
	}

	// Integer, Signed-Signed, CanFit
	template <typename F, typename T>
	inline
	std::enable_if_t<
		std::conjunction_v<
			std::negation<std::is_same<F, T>>,
			std::conjunction<
				is_integral_not_bool<F>,
				is_integral_not_bool<T>
			>,
			std::conjunction<
				std::is_signed<F>,
				std::is_signed<T>
			>,
			can_fit_in<F, T>
		>,
		bool>
	Convert(const F& from, T& to)
	{
		to = static_cast<T>(from);
		return true;
	}

	// Integer, Unsigned-Unsigned, CanFit
	template <typename F, typename T>
	inline
	std::enable_if_t<
		std::conjunction_v<
			std::negation<std::is_same<F, T>>,
			std::conjunction<
				is_integral_not_bool<F>,
				is_integral_not_bool<T>
			>,
			std::conjunction<
				std::is_unsigned<F>,
				std::is_unsigned<T>
			>,
			can_fit_in<F, T>
		>,
		bool>
	Convert(const F& from, T& to)
	{
		to = static_cast<T>(from);
		return true;
	}

	// Integer, Signed-Signed, CannotFit
	template <typename F, typename T>
	inline
	std::enable_if_t<
		std::conjunction_v<
			std::negation<std::is_same<F, T>>,
			std::conjunction<
				is_integral_not_bool<F>,
				is_integral_not_bool<T>
			>,
			std::conjunction<
				std::is_signed<F>,
				std::is_signed<T>
			>,
			std::negation<can_fit_in<F, T>>
		>,
		bool>
	Convert(const F& from, T& to)
	{
		if (from > std::numeric_limits<T>::max() || from < std::numeric_limits<T>::min())
			return false;
		
		to = static_cast<T>(from);
		return true;
	}

	// Integer, Unsigned-Unsigned, CannotFit
	template <typename F, typename T>
	inline
	std::enable_if_t<
		std::conjunction_v<
			std::negation<std::is_same<F, T>>,
			std::conjunction<
				is_integral_not_bool<F>,
				is_integral_not_bool<T>
			>,
			std::conjunction<
				std::is_unsigned<F>,
				std::is_unsigned<T>
			>,
			std::negation<can_fit_in<F, T>>
		>,
		bool>
	Convert(const F& from, T& to)
	{
		if (from > std::numeric_limits<T>::max())
			return false;
		
		to = static_cast<T>(from);
		return true;
	}

	// Integer, Unsigned-Signed
	template <typename F, typename T>
	inline
	std::enable_if_t<
		std::conjunction_v<
			std::negation<std::is_same<F, T>>,
			std::conjunction<
				is_integral_not_bool<F>,
				is_integral_not_bool<T>
			>,
			std::conjunction<
				std::is_unsigned<F>,
				std::is_signed<T>
			>
		>,
		bool>
	Convert(const F& from, T& to)
	{
		if (from > static_cast<std::make_unsigned_t<T>>(std::numeric_limits<T>::max()))
			return false;
		
		to = static_cast<T>(from);
		return true;
	}

	// Integer, Signed-Unsigned
	template <typename F, typename T>
	inline
	std::enable_if_t<
		std::conjunction_v<
			std::negation<std::is_same<F, T>>,
			std::conjunction<
				is_integral_not_bool<F>,
				is_integral_not_bool<T>
			>,
			std::conjunction<
				std::is_signed<F>,
				std::is_unsigned<T>
			>
		>,
		bool>
	Convert(const F& from, T& to)
	{
		if (from < 0)
			return false;

		if (static_cast<std::make_unsigned_t<F>>(from) > std::numeric_limits<T>::max())
			return false;

		to = static_cast<T>(from);
		return true;
	}
}