#include "Field.h"
#include "Class.h"

const char* Field::Name() const
{
    return m_Name;
}

::Type* Field::Type() const
{
    return m_Type;
}

size_t Field::Offset() const
{
    return m_Offset;
}

Any Field::GetValue(Any& o) const
{
    if (o.m_Type == static_cast<::Type* const>(m_ParentClass)) {
        auto value = Any::From(reinterpret_cast<uint8_t*>(o.Ptr()) + m_Offset, m_Type);
        return value;
    }
    return {};
}