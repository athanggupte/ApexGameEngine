#pragma once
#include "Function.h"

#include <functional>


enum class InvokePolicy
{
    eDefaultInvoke,
    // TODO: Implement invoke policies
    eDiscardReturn,
    eReturnAsPtr
};

struct in_tuple_t {
    explicit in_tuple_t() = default;
};

template <typename F, typename... Args>
struct IsInvocable : std::is_invocable<F, Args...> {};

template <typename F, typename... Args>
struct IsInvocable<F, in_tuple_t, std::tuple<Args...>> : IsInvocable<F, Args...> {};


/////////////////////////////////////////////////////////////////////////
template <typename T>
struct FunctionTraits : FunctionTraits<decltype(&T::operator())> {};

template <typename R, typename... Args>
struct FunctionTraits<R (Args...)>
{
    static constexpr size_t ArgCount = sizeof...(Args);
    static constexpr bool IsMember = false;

    using ReturnType = R;
    using ArgTypes = std::tuple<Args...>;
};

template <typename R, typename... Args>
struct FunctionTraits<R (*)(Args...)> : FunctionTraits<R (Args...)> {};

template <typename R, typename... Args>
struct FunctionTraits<R (&)(Args...)> : FunctionTraits<R (Args...)> {};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...)> : FunctionTraits<R (Args...)> { using ClassType = C; static constexpr bool IsMember = true; };

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const> : FunctionTraits<R (Args...)> { using ClassType = C; static constexpr bool IsMember = true; };

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) volatile> : FunctionTraits<R (Args...)> { using ClassType = C; static constexpr bool IsMember = true; };

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const volatile> : FunctionTraits<R (Args...)> { using ClassType = C; static constexpr bool IsMember = true; };

template <typename T>
struct FunctionTraits<std::function<T>> : FunctionTraits<T> {};


template <typename F, size_t ArgIndex>
struct ArgType
{
    using Type = typename std::tuple_element_t<ArgIndex, typename FunctionTraits<F>::ArgTypes>;
};


namespace function_type {

    struct VoidFunc
    {
        using Type = VoidFunc;
        static constexpr std::string_view TypeName = "VoidFunc";
    };

    struct ReturnFunc
    {
        using Type = ReturnFunc;
        static constexpr std::string_view TypeName = "ReturnFunc";
    };

    struct VoidMemberFunc
    {
        using Type = VoidMemberFunc;
        static constexpr std::string_view TypeName = "VoidMemberFunc";
    };

    struct ReturnMemberFunc
    {
        using Type = ReturnMemberFunc;
        static constexpr std::string_view TypeName = "ReturnMemberFunc";
    };

}

namespace detail {

    template <typename F>
    struct FunctionType : std::conditional_t<
                                std::is_member_function_pointer_v<F>,
                                std::conditional_t<
                                    std::is_void_v<typename FunctionTraits<F>::ReturnType>,
                                    ::function_type::VoidMemberFunc,
                                    ::function_type::ReturnMemberFunc
                                >,
                                std::conditional_t<
                                    IsInvocable<F, in_tuple_t, typename FunctionTraits<F>::ArgTypes>::value,
                                    std::conditional_t<
                                        std::is_void_v<typename FunctionTraits<F>::ReturnType>,
                                        ::function_type::VoidFunc,
                                        ::function_type::ReturnFunc
                                    >,
                                    void
                                >
                            >
    {};

}