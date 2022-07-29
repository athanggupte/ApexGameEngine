#pragma once
#include "FunctionTraits.h"
#include "FunctionInvoker.h"

namespace detail {

    template <typename F, InvokePolicy Policy>
    struct FunctionAccessor
    {
        static constexpr size_t ArgCount = FunctionTraits<F>::ArgCount;
        using FunctionType = typename FunctionType<F>::Type;
        using ArgIndexSequence = std::make_index_sequence<ArgCount>;
        using Invoker = FunctionInvoker<F, Policy, FunctionType, ArgIndexSequence>;

        //template <typename... TArgs>
        //static inline
        //std::enable_if_t<sizeof...(TArgs) == ArgCount, Any>
        //Invoke(const F& func, const Any& obj, TArgs... args)
        //{
        //    return Invoker::Invoke(func, obj, args...);
        //}

        //template <typename... TArgs>
        //static inline
        //std::enable_if_t<sizeof...(TArgs) != ArgCount, Any>
        //Invoke(const F& func, const Any& obj, TArgs... args)
        //{
        //    //static_assert(sizeof...(TArgs) == ArgCount, "Function invoked with invalid number of arguments!");
        //    //(void)assert_equality_v<sizeof...(TArgs), ArgCount>;

        //    return Any{};
        //}

        template <typename... TArgs>
        static inline
        std::enable_if_t<sizeof...(TArgs) == ArgCount, Any>
        Invoke(const F& func, Any& obj, TArgs... args)
        {
            return Invoker::Invoke(func, obj, args...);
        }

        template <typename... TArgs>
        static inline
        std::enable_if_t<sizeof...(TArgs) != ArgCount, Any>
        Invoke(const F& func, Any& obj, TArgs... args)
        {
            //static_assert(sizeof...(TArgs) == ArgCount, "Function invoked with invalid number of arguments!");
            //(void)assert_equality_v<sizeof...(TArgs), ArgCount>;

            return Any{};
        }


        static inline
        Any InvokeVariadic(const F& func, const Any& obj, const std::vector<Any>& vargs)
        {
            if (vargs.size() == ArgCount) {
                InvokeVariadicImpl(func, obj, std::make_index_sequence<ArgCount>{}, vargs);
            } else {
                return Any{};
            }
        }

        template <size_t... Arg_Idx>
        static inline
        Any InvokeVariadicImpl(const F& func, const Any& obj, std::index_sequence<Arg_Idx...>, const std::vector<Any>& vargs)
        {
            return Invoker::Invoke(func, obj, vargs[Arg_Idx]...);
        }

    };

}