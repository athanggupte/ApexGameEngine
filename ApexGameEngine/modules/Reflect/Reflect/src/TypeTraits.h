#pragma once
#include <type_traits>

template <typename T>
struct remove_all_pointers
{
    using Type = T;
};

template <typename T>
struct remove_all_pointers<T*>
{
    using Type = typename remove_all_pointers<T>::Type;
};

template <typename T>
struct remove_all_pointers<T* const>
{
    using Type = typename remove_all_pointers<T>::Type;
};

template <typename T>
struct remove_all_pointers<T* volatile>
{
    using Type = typename remove_all_pointers<T>::Type;
};

template <typename T>
struct remove_all_pointers<T* const volatile>
{
    using Type = typename remove_all_pointers<T>::Type;
};

template <typename T>
using remove_all_pointers_t = typename remove_all_pointers<T>::Type;

template <typename T>
struct full_decay
{
    using Type = remove_all_pointers_t<std::decay_t<remove_all_pointers_t<T>>>;
};

template <typename T>
using full_decay_t = typename full_decay<T>::Type;

template <typename T>
using remove_all_const_t = std::remove_const_t<typename std::remove_pointer_t<T>>*;

template <typename T1, typename T2>
struct can_fit_in : std::bool_constant<sizeof(T1) <= sizeof(T2)> {};

template <typename T>
struct is_integral_not_bool : std::conjunction<
			                        std::negation<std::is_same<T, bool>>,
			                        std::is_integral<T>
		                        >
{};

/////////////////////////////////////////////////////////////////////////
#include <vector>
#include <memory>

/////////////////////////////////////////////////////////////////////////
// Smart pointers
template <typename T>
struct IsSmartPointer : std::conditional_t<
                            std::is_same_v<full_decay_t<T>, T>,
                            std::false_type,
                            IsSmartPointer<full_decay_t<T>>
                        >
{};

template <typename T>
struct IsSmartPointer<std::shared_ptr<T>> : std::true_type {};

template <typename T>
struct IsSmartPointer<std::unique_ptr<T>> : std::true_type {};

/////////////////////////////////////////////////////////////////////////
// Vector
template <typename T>
struct IsVector : std::conditional_t<
                        std::is_same_v<full_decay_t<T>, T>,
                        std::false_type,
                        IsVector<full_decay_t<T>>
                    >
{};

template <typename T>
struct IsVector<std::vector<T>> : std::true_type {};

// Pair
template <typename T>
struct IsPair : std::conditional_t<
                        std::is_same_v<full_decay_t<T>, T>,
                        std::false_type,
                        IsPair<full_decay_t<T>>
                    >
{};

template <typename T, typename U>
struct IsPair<std::pair<T, U>> : std::true_type {};

// String
template <typename T>
struct IsString : std::conditional_t<
                        std::disjunction_v<
                            std::is_same<std::string, T>,
                            std::is_same<std::wstring, T>
                        >,
                        std::true_type,
                        std::false_type
                    >
{};

/////////////////////////////////////////////////////////////////////////
template <typename T>
struct IsBuiltInType : std::conditional_t<
                            std::disjunction_v<
                                std::is_fundamental<T>,
                                IsSmartPointer<T>,
                                IsVector<T>,
                                IsPair<T>,
                                IsString<T>
                                >,
                            std::true_type,
                            std::false_type
                        >
{};

/////////////////////////////////////////////////////////////////////////