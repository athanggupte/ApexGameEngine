#define SKIP_TYPE_DECLARATIONS
#include "FunctionImplBase.h"

namespace detail {
    FunctionImplBase::FunctionImplBase(std::string_view name, Class* parent_class)
        : m_Name(name), m_ParentClass(parent_class)
    {
    }

    Any FunctionImplBase::Invoke(Any target) const
    {
        return Any{};
    }

    Any FunctionImplBase::Invoke(Any target, Any arg1) const
    {
        return Any{};
    }

    Any FunctionImplBase::Invoke(Any target, Any arg1, Any arg2) const
    {
        return Any{};
    }

    Any FunctionImplBase::Invoke(Any target, Any arg1, Any arg2, Any arg3) const
    {
        return Any{};
    }

    Any FunctionImplBase::Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4) const
    {
        return Any{};
    }

    Any FunctionImplBase::Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5) const
    {
        return Any{};
    }

    Any FunctionImplBase::Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5, Any arg6) const
    {
        return Any{};
    }

    Any FunctionImplBase::InvokeVariadic(Any target, std::vector<Any>& args) const
    {
        return Any();
    }

    std::string_view FunctionImplBase::Name() const
    {
        return m_Name;
    }

    ::Type* FunctionImplBase::ReturnType() const
    {
        return nullptr;
    }

    ArrayRange<ParameterInfo> FunctionImplBase::Parameters() const
    {
        return {};
    }

    Class* FunctionImplBase::ParentClass() const
    {
        return m_ParentClass;
    }
}