#include "Function.h"
#include "FunctionImplBase.h"

Function::Function()
    : m_Impl(nullptr)
{
}

Function::Function(const detail::FunctionImplBase* impl)
    : m_Impl(impl)
{
}

Function::Function(const Function& other)
    : m_Impl(other.m_Impl)
{
}

Function::Function(Function&& other)
    : m_Impl(std::move(other.m_Impl))
{
}

Function& Function::operator=(const Function& other)
{
    m_Impl = other.m_Impl;
    return *this;
}

Function& Function::operator=(Function&& other)
{
    m_Impl = std::move(other.m_Impl);
    return *this;
}

std::string_view Function::Name() const
{
    return m_Impl->Name();
}

Type* Function::ReturnType() const
{
    return m_Impl->ReturnType();
}

ArrayRange<ParameterInfo> Function::Parameters() const
{
    return m_Impl->Parameters();
}

Class* Function::ParentClass() const
{
    return m_Impl->ParentClass();
}

bool Function::IsValid() const
{
    return static_cast<bool>(m_Impl);
}

Any Function::Invoke(Any target) const
{
    return m_Impl->Invoke(target);
}

Any Function::Invoke(Any target, Any arg1) const
{
    return m_Impl->Invoke(target, arg1);
}

Any Function::Invoke(Any target, Any arg1, Any arg2) const
{
    return m_Impl->Invoke(target, arg1, arg2);
}

Any Function::Invoke(Any target, Any arg1, Any arg2, Any arg3) const
{
    return m_Impl->Invoke(target, arg1, arg2, arg3);
}

Any Function::Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4) const
{
    return m_Impl->Invoke(target, arg1, arg2, arg3, arg4);
}

Any Function::Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5) const
{
    return m_Impl->Invoke(target, arg1, arg2, arg3, arg4, arg5);
}

Any Function::Invoke(Any target, Any arg1, Any arg2, Any arg3, Any arg4, Any arg5, Any arg6) const
{
    return m_Impl->Invoke(target, arg1, arg2, arg3, arg4, arg5, arg6);
}

Any Function::InvokeVariadic(Any target, std::vector<Any>& args) const
{
    return m_Impl->InvokeVariadic(target, args);
}
