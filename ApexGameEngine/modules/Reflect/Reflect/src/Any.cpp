#include "Any.h"
#include "Type.h"

Any::Any(std::reference_wrapper<Any> ref)
	: m_Type(ref.get().m_Type), m_Ptr(ref.get().m_Ptr), m_Owned(false)
{
}

Any::Any(const Any& other)
    : m_Owned(other.m_Owned)
{
    m_Type = other.m_Type;
    if (m_Owned)
        m_Ptr = m_Type->_GetMemoryFunctions()->_CopyPtr(other.m_Ptr);
    else
        m_Ptr = other.m_Ptr;
}

Any::Any(Any&& other)
	: m_Owned(other.m_Owned)
{
	m_Type = other.m_Type;
	m_Ptr = other.m_Ptr;
	other.m_Owned = false;
}

Any::~Any()
{
	Reset();
}

Any& Any::operator=(const Any& other)
{
	*this = Any{ other };
	return *this;
}

Any& Any::operator=(Any&& other)
{
	Reset();
	m_Type = other.m_Type;
	m_Owned = other.m_Owned;
	m_Ptr = other.m_Ptr;
	other.m_Owned = false;
	return *this;
}

Any& Any::operator=(std::reference_wrapper<Any> ref)
{
	m_Owned = false;
	m_Type = ref.get().m_Type;
	m_Ptr = ref.get().m_Ptr;
	return *this;
}

Type* Any::Type() const
{
	return m_Type;
}

bool Any::IsValid() const
{
    return m_Type || m_Ptr;
}

bool Any::HasValue() const
{
	return static_cast<bool>(m_Ptr);
}

void Any::Reset()
{
    if (m_Owned && m_Ptr) {
        m_Type->_GetMemoryFunctions()->_DestroyPtr(m_Ptr);
    }
    m_Ptr = nullptr;
    m_Type = nullptr;
}

Any::operator bool() const
{
	return HasValue();
}

Any Any::Reference() const
{
	return Any{ std::ref(const_cast<Any&>(*this)) };
}

Any Any::From(void* ptr, ::Type* type)
{
	Any any;
	any._CreateFrom(ptr, type);
	return any;
}

bool Any::_ValueCast(Any& to) const
{
    return m_Type->_Convert(*this, to);
}

void Any::_CreateFrom(void* ptr, ::Type* type)
{
	m_Type = type;
	m_Ptr = ptr;
	m_Owned = false;
}

void* Any::Ptr()
{
	return m_Ptr;
}

const void* Any::Ptr() const
{
	return m_Ptr;
}
