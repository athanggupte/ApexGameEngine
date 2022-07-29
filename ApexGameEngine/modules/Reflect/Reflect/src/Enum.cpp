#include "Enum.h"
#include "Type.h"

bool Enum::_Convert(const Any& from, Any& to) const
{
    return GetUnderlyingType()->_Convert(from, to);
}

EnumIterator Enum::enum_begin()
{
    return EnumIterator{ 0, *this };
}

EnumIterator Enum::enum_end()
{
    return EnumIterator{ m_NumValues, *this };
}