#include "Any.h"
#include "ParameterInfo.h"
#include "ParameterInfoBase.h"

ParameterInfo::ParameterInfo(const detail::ParameterInfoBase* impl)
	: m_Impl(impl)
{
}

std::string_view ParameterInfo::Name() const
{
	return m_Impl->Name();
}

::Type* ParameterInfo::Type() const
{
	return m_Impl->Type();
}

size_t ParameterInfo::Index() const
{
	return m_Impl->Index();
}

bool ParameterInfo::HasDefaultValue() const
{
	return m_Impl->HasDefaultValue();
}

Any ParameterInfo::GetDefaultValue() const
{
	return m_Impl->GetDefaultValue();
}
