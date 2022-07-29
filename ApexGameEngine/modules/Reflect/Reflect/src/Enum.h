#pragma once
#include "config.h"
#include "Any.h"
#include "Type.h"

struct EnumIterator;

struct REFLECT_API Enum : public Type
{
// Type-info accessors
    virtual std::string_view NameOf(const Any& value) = 0;
	virtual Type* GetUnderlyingType() const = 0;

    Reflect::TypeInfo GetTypeInfo() const
    {
        return Reflect::TypeInfo::eEnum;
    }
    
    struct ValuePair
	{
		std::string_view name;
		Any value;
	};

    EnumIterator enum_begin();
    EnumIterator enum_end();

protected:
    size_t m_NumValues;
	std::string_view* names_beg;
	std::string_view* names_end;

    virtual const std::string_view* GetNames() const = 0;
    virtual Any GetValueAt(size_t idx) const = 0;
	
	bool _Convert(const Any& from, Any& to) const override;

    friend struct EnumIterator;
};

struct REFLECT_API EnumIterator
{
	size_t idx;
    const Enum& _enum;

	Enum::ValuePair Get() const
	{
		return { _enum.GetNames()[idx], _enum.GetValueAt(idx) };
	}

	Enum::ValuePair operator * () const
	{
		return Get();
	}

	EnumIterator& operator ++ ()
	{
		++idx;
		return *this;
	}

	EnumIterator& operator -- ()
	{
		--idx;
		return *this;
	}

	EnumIterator& operator ++ (int)
	{
		auto& _this = *this;
		++idx;
		return _this;
	}

	EnumIterator& operator -- (int)
	{
		auto& _this = *this;
		--idx;
		return _this;
	}

    bool operator == (const EnumIterator& other) const
    {
        return &_enum == &other._enum && idx == other.idx;
    }

    bool operator != (const EnumIterator& other) const
    {
        return !(*this == other);
    }
};