#pragma once
#include "config.h"
#include "Type.h"
#include "Any.h"
#include "Utils.h"
#include "ParameterInfo.h"

namespace detail {
    struct FunctionImplBase;
}

struct REFLECT_API Function
{
    Function();
    Function(const detail::FunctionImplBase* impl);

    Function(const Function&);
    Function(Function&&);

    Function& operator = (const Function&);
    Function& operator = (Function&&);

// Type-info accessors
    std::string_view Name() const;
    Type* ReturnType() const;
    ArrayRange<ParameterInfo> Parameters() const;
    Class* ParentClass() const;

    bool IsValid() const;

    Any Invoke(Any target) const;
    Any Invoke(Any target, Any arg1) const;
    Any Invoke(Any target, Any arg1, Any arg2) const;
    Any Invoke(Any target, Any arg1, Any arg2, Any arg3) const;
    Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4) const;
    Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5) const;
    Any Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5, Any arg6) const;
    Any InvokeVariadic(Any target, std::vector<Any>& args) const;
    
// Members

private:
    const detail::FunctionImplBase* m_Impl;
};