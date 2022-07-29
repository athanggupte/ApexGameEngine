#pragma once
//#include "Type.h"
#include <string>
#include "TypeTraits.h"
#include "config.h"

struct Type;

class REFLECT_API Any
{
public:
    constexpr Any() {}

    template <typename ValueType, typename = std::enable_if_t<std::conjunction_v<
                                                std::negation<std::is_same<std::decay_t<ValueType>, Any>>,
                                                std::negation<std::_Is_specialization<std::decay_t<ValueType>, std::in_place_type_t>>,
                                                std::is_copy_constructible<std::decay_t<ValueType>>
                                            >>>
    Any(ValueType&& value)
    {
        _Emplace<std::decay_t<ValueType>>(std::forward<ValueType>(value));
    }

    template <typename ValueType, typename = std::enable_if_t<std::negation_v<std::is_same<std::decay_t<ValueType>, Any>>>>
    Any(std::reference_wrapper<ValueType> ref)
    {
        _CreateFrom(std::addressof(ref.get()), GetType<ValueType>());
    }

    Any(std::reference_wrapper<Any> ref);
    Any(const Any& other);
    Any(Any&& other);

    ~Any();

    template <typename ValueType, typename = std::enable_if_t<std::conjunction_v<
												std::negation<std::is_same<std::decay_t<ValueType>, Any>>,
									            std::is_copy_constructible<std::decay_t<ValueType>>
											>>>
    Any& operator = (ValueType&& value)
    {
        *this = Any{ value };
        return *this;
    }

	Any& operator = (const Any& other);
    Any& operator = (Any&& other);
    Any& operator = (std::reference_wrapper<Any> ref);

    template <typename ValueType, typename... Args>
    std::decay_t<ValueType>& Emplace(Args&&... args)
    {
        Reset();
        return _Emplace<std::decay_t<ValueType>>(std::forward<Args>(args)...);
    }

    [[nodiscard]] Type* Type() const;

    template <typename T>
    [[nodiscard]] bool IsType() const
    {
        return Type() == GetType<T>();
    }

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] bool HasValue() const;

    void Reset();

    operator bool() const;

    Any Reference() const;

    template <typename ValueType>
    [[nodiscard]] bool CanConvert() const
    {
        return _CanConvert<std::decay_t<ValueType>>();
    }

    template <typename ValueType>
    bool Value(ValueType& to)
    {
        return _ValueCast(Any{ std::ref(to) });
    }

    template <typename ValueType>
    std::decay_t<ValueType>& Cast()
    {
        return *_Cast<std::decay_t<ValueType>>();
    }

    template <typename ValueType>
    const std::decay_t<ValueType>& Cast() const
    {
        return *_Cast<std::decay_t<ValueType>>();
    }

    template <typename ValueType>
    std::decay_t<ValueType>* CastPtr()
    {
        return _Cast<std::decay_t<ValueType>>();
    }

    template <typename ValueType>
    const std::decay_t<ValueType>* CastPtr() const
    {
        return _Cast<std::decay_t<ValueType>>();
    }

    static Any From(void* ptr, ::Type* type);

protected:
    template <typename DecayedType, typename... Args>
    DecayedType& _Emplace(Args&&... args)
    {
        m_Type = GetType<DecayedType>();
        DecayedType* const ptr = new DecayedType{ std::forward<Args>(args)... };
        m_Ptr = ptr;
        return *ptr;
    }

    template <typename DecayedType>
    const DecayedType* _Cast() const;

    bool _ValueCast(Any& to) const;

    template <typename DecayedType>
    DecayedType* _Cast();

    template <typename DecayedType>
    bool _CanConvert();

    void _CreateFrom(void* ptr, ::Type* type);

    [[nodiscard]] void* Ptr();
    [[nodiscard]] const void* Ptr() const;

private:
    ::Type* m_Type = nullptr;
    void* m_Ptr = nullptr;
    bool m_Owned = true;


    friend struct Type;
    friend struct Class;
    friend struct Field;
    friend struct Function;
};

#include "Enum.h"

template <typename DecayedType>
const DecayedType* Any::_Cast() const
{
	if (GetType<DecayedType>() == m_Type) {
		return static_cast<const DecayedType*>(m_Ptr);
	}
	if (Enum* e = m_Type->CastAs<Enum>(); e && e->GetUnderlyingType() == GetType<DecayedType>()) {
		return static_cast<const DecayedType*>(m_Ptr);
	}
	return nullptr;
}

template <typename DecayedType>
DecayedType* Any::_Cast()
{
	return const_cast<DecayedType*>(static_cast<const Any*>(this)->_Cast<DecayedType>());
}

template <typename DecayedType>
bool Any::_CanConvert()
{
	if (!IsValid()) return false;

	if (std::is_arithmetic_v<DecayedType> && m_Type->IsArithmetic())
		return true;

	if (std::is_integral_v<DecayedType> && m_Type->IsEnum())
		return true;

	if (std::is_arithmetic_v<DecayedType> && m_Type->IsString())
		return std::strtol(m_Ptr);

	return GetType<DecayedType>() == m_Type;
}
