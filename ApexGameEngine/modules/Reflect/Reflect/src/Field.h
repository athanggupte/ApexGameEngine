#pragma once
#include "config.h"
#include "Type.h"
#include "Any.h"
//#include <entt/entt.hpp>

struct REFLECT_API Field
{
// Type-info accessors
    const char* Name() const;
    ::Type* Type() const;
    size_t Offset() const;

    template <typename T, std::enable_if_t<std::is_class_v<T>>>
    Any GetValue(T& target) const
    {
        ::Type* type = GetType<T>();
        return GetValue(Any::From(reinterpret_cast<void*>(&target), type));
        // if (type == static_cast<::Type*>(m_ParentClass)) {
        //     auto value = Any::From(reinterpret_cast<uint8_t*>(&target) + m_Offset, m_Type);
        //     return value;
        // }
        // return {};
    }
    
    Any GetValue(Any& o) const;

// Members
    ::Type* m_Type = nullptr;
    char const* m_Name = nullptr;
    size_t m_Offset = 0;
    Class* m_ParentClass = nullptr;

    //entt::entity id;
};