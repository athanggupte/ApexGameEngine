#pragma once
#include "Type.h"
#include <string_view>

namespace detail {
	
    struct ParameterInfoBase
    {
        virtual ~ParameterInfoBase() = default;
        virtual std::string_view Name() const = 0;
        virtual ::Type* Type() const = 0;
        virtual size_t Index() const = 0;
        virtual bool HasDefaultValue() const = 0;
        virtual Any GetDefaultValue() const = 0;
    };

}