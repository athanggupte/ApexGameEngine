#pragma once
#include "FunctionImplBase.h"
#include "FunctionTraits.h"
#include "FunctionArgs.h"
#include "FunctionAccessor.h"
#include "TypeTraits.h"
#include "ParamerInfoImpl.h"

namespace detail {

    template <typename F, InvokePolicy Policy, typename Param_Infos/*, typename Default_Args*/>
    struct FunctionImpl;

    template <typename F>
    Class* _GetParentClass()
    {
        if constexpr (FunctionTraits<F>::IsMember)
            return GetClass<typename FunctionTraits<F>::ClassType>();
        else
            return nullptr;
    }

    template <typename F, InvokePolicy Policy, typename... Param_Infos>
    struct FunctionImpl<F, Policy, ParamInfos<Param_Infos...>/*, DefaultArgs<>*/> : public FunctionImplBase
    {
        static_assert(sizeof...(Param_Infos) == FunctionTraits<F>::ArgCount, "Invalid Parameter Info count!");

        FunctionImpl(std::string_view name, F func, ParamInfos<Param_Infos...> param_infos, Class* parentClass = _GetParentClass<F>())
            : FunctionImplBase(name, parentClass),
              m_Func(func),
              m_ParamInfos(std::move(param_infos)),
              m_ParamInfoArr(CreateParamInfoArray(m_ParamInfos))
        {
        }

        Type* ReturnType() const override { return GetType<FunctionTraits<F>::ReturnType>(); }
        ArrayRange<ParameterInfo> Parameters() const override { return ArrayRange{ m_ParamInfoArr.data(), m_ParamInfoArr.data() + m_ParamInfoArr.size() }; }

        Any Invoke(Any target) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target);
        }

        Any Invoke(Any target, Any arg1) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1);
        }

        Any Invoke(Any target, Any arg1, Any arg2) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3, arg4);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3, arg4, arg5);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5, Any arg6) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3, arg4, arg5);
        }
    
        Any Invoke(Any target, std::vector<Any>& args) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, args);
        }

        F m_Func;
        ParamInfos<Param_Infos...> m_ParamInfos;
        std::array<ParameterInfo, sizeof...(Param_Infos)> m_ParamInfoArr;
    };


    template <typename F, InvokePolicy Policy>
    struct FunctionImpl<F, Policy, ParamInfos<>/*, DefaultArgs<>*/> : public FunctionImplBase
    {
        static_assert(0 == FunctionTraits<F>::ArgCount, "Invalid Parameter Info count!");

        FunctionImpl(std::string_view name, F func, ParamInfos<>, Class* parentClass = _GetParentClass<F>())
            : FunctionImplBase(name, parentClass),
              m_Func(func)
        {
        }

        Type* ReturnType() const override { return GetType<FunctionTraits<F>::ReturnType>(); }

        Any Invoke(Any target) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target);
        }

        Any Invoke(Any target, Any arg1) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1);
        }

        Any Invoke(Any target, Any arg1, Any arg2) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3, arg4);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3, arg4, arg5);
        }
    
        Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5, Any arg6) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, arg1, arg2, arg3, arg4, arg5);
        }
    
        Any Invoke(Any target, std::vector<Any>& args) const
        {
            return FunctionAccessor<F, Policy>::Invoke(m_Func, target, args);
        }

        F m_Func;
    };

    template <typename F, InvokePolicy Policy, typename... Param_Infos>
    static inline FunctionImpl<F, Policy, ParamInfos<Param_Infos...>>
    CreateFunctionImpl(std::string_view);


}