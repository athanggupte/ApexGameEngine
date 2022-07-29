#pragma once
#include "FunctionTraits.h"
#include "Any.h"
#include "Utils.h"

namespace detail {

    template <typename F, InvokePolicy Policy, typename Type, typename IndexSequence>
    struct FunctionInvoker;

    template <typename F, size_t... ArgCount>
    struct FunctionInvoker<F, InvokePolicy::eDefaultInvoke, function_type::VoidFunc, std::index_sequence<ArgCount...>>
    {
        template <typename... TArgs>
        inline static Any Invoke(const F& func, const Any& obj, const TArgs&... args)
        {
            static_assert(sizeof...(ArgCount) == sizeof...(TArgs), "Function invoked with invalid number of arguments!");
        
            if (AllTrue(args.template IsType<ArgType<F, ArgCount>::Type>()...)) {
                func(args.template Cast<ArgType<F, ArgCount>::Type>()...);
                return Void{};
            } else {
                return Any{};
            }
        }
    };

    template <typename F, size_t... ArgCount>
    struct FunctionInvoker<F, InvokePolicy::eDefaultInvoke, function_type::ReturnFunc, std::index_sequence<ArgCount...>>
    {
        template <typename... TArgs>
        inline static Any Invoke(const F& func, const Any& obj, const TArgs&... args)
        {
            static_assert(sizeof...(ArgCount) == sizeof...(TArgs), "Function invoked with invalid number of arguments!");

            if (AllTrue(args.template IsType<ArgType<F, ArgCount>::Type>()...)) {
                return func(args.template Cast<ArgType<F, ArgCount>::Type>()...);
            } else {
                return Any{};
            }
        }
    };

    template <typename F, size_t... ArgCount>
    struct FunctionInvoker<F, InvokePolicy::eDefaultInvoke, function_type::VoidMemberFunc, std::index_sequence<ArgCount...>>
    {
        template <typename... TArgs>
        inline static Any Invoke(const F& func_ptr, const Any& obj, const TArgs&... args)
        {
            static_assert(sizeof...(ArgCount) == sizeof...(TArgs), "Function invoked with invalid number of arguments!");
            using ClassT = typename FunctionTraits<F>::ClassType;
            ClassT* ptr = const_cast<ClassT*>(obj.CastPtr<ClassT>());
            if (ptr && AllTrue(args.template IsType<ArgType<F, ArgCount>::Type>()...)) {
                (ptr->*func_ptr)(args.template Cast<ArgType<F, ArgCount>::Type>()...);
                return Void{};
            } else {
                return Any{};
            }
        }

        //template <typename... TArgs>
        //inline static Any Invoke(const F& func_ptr, Any& obj, const TArgs&... args)
        //{
        //    static_assert(sizeof...(ArgCount) == sizeof...(TArgs), "Function invoked with invalid number of arguments!");
        //    using ClassT = typename FunctionTraits<F>::ClassType;
        //    ClassT* ptr = obj.CastPtr<ClassT>();
        //    if (ptr && AllTrue(args.IsType<ArgType<F, ArgCount>::Type>()...)) {
        //        (ptr->*func_ptr)(args.Cast<ArgType<F, ArgCount>::Type>()...);
        //        return Void{};
        //    } else {
        //        return Any{};
        //    }
        //}

    };

    template <typename F, size_t... ArgCount>
    struct FunctionInvoker<F, InvokePolicy::eDefaultInvoke, function_type::ReturnMemberFunc, std::index_sequence<ArgCount...>>
    {
        template <typename... TArgs>
        inline static Any Invoke(const F& func_ptr, const Any& obj, const TArgs&... args)
        {
            static_assert(sizeof...(ArgCount) == sizeof...(TArgs), "Function invoked with invalid number of arguments!");
            using ClassT = typename FunctionTraits<F>::ClassType;
            ClassT* ptr = const_cast<ClassT*>(obj.CastPtr<ClassT>());
            if (ptr && AllTrue(args.template IsType<ArgType<F, ArgCount>::Type>()...)) {
                return (ptr->*func_ptr)(args.template Cast<ArgType<F, ArgCount>::Type>()...);
            } else {
                return Any{};
            }
        }
    };

}
