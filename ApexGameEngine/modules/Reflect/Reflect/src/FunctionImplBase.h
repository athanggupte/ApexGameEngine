#pragma once
#include "Any.h"
#include "Utils.h"

struct ParameterInfo;

namespace detail {
    struct FunctionImplBase
    {
        FunctionImplBase(std::string_view name, Class* parent_class);

        virtual ~FunctionImplBase() = default;

        virtual Any Invoke(Any target) const;
        virtual Any Invoke(Any target, Any arg1) const;
        virtual Any Invoke(Any target, Any arg1, Any arg2) const;
        virtual Any Invoke(Any target, Any arg1, Any arg2, Any arg3) const;
        virtual Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4) const;
        virtual Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5) const;
        virtual Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5, Any arg6) const;
        virtual Any InvokeVariadic(Any target, std::vector<Any>& args) const;

        virtual std::string_view Name() const;
        virtual ::Type* ReturnType() const;
        virtual ArrayRange<ParameterInfo> Parameters() const;
        virtual Class* ParentClass() const;

    protected:
        std::string_view m_Name;
        Class* m_ParentClass = nullptr;
    };
}