#pragma once
#include <string_view>

struct Type;
class Any;

namespace detail {
    struct ParameterInfoBase;
}

struct ParameterInfo
{
    ParameterInfo(const detail::ParameterInfoBase* impl);

    std::string_view Name() const;
    ::Type* Type() const;
    size_t Index() const;

    bool HasDefaultValue() const;
    Any GetDefaultValue() const;

private:
    const detail::ParameterInfoBase* m_Impl;
};