#pragma once
#include "config.h"
#include "Type.h"
#include "Utils.h"
#include "Field.h"
#include "Function.h"

namespace detail {
    struct FunctionImplBase;
}

struct REFLECT_API Class : public Type
{
    Class() = default;
// Type-info accessors
    //const std::vector<Field>& Fields() const;
    //const std::vector<Function>& Methods() const;

    using FieldRange    = ArrayRange<Field>;
    using FunctionRange = ArrayRange<Function>;
    using ClassRange    = ArrayRange<Class*>;

    using FieldPtr    = FieldRange::pointer_type;
    using FunctionPtr = FunctionRange::pointer_type;
    using ClassPtr    = ClassRange::pointer_type;

    FieldRange fields() const { return { m_field_beg, m_field_end }; }
    FunctionRange functions() const { return { m_func_beg, m_func_end }; }
    ClassRange bases() const { return { m_bases_beg, m_bases_end }; }
    ClassRange derived() const { return { m_Derived.data(), m_Derived.size() }; }

    Field& AddField();
    Function& AddMethod(const detail::FunctionImplBase* impl);

    Reflect::TypeInfo GetTypeInfo() const override
    {
        return Reflect::TypeInfo::eClass;
    }

    bool IsBaseOf(Class* other) const;

protected:
    void _InitializeStorage(FieldPtr field_beg, FieldPtr field_end,
        FunctionPtr func_beg, FunctionPtr func_end,
        ClassPtr bases_beg, ClassPtr bases_end);

    bool _Convert(const Any& from, Any& to) const override;

// Members
    FieldPtr m_field_beg;
    FieldPtr m_field_end;

    FunctionPtr m_func_beg;
    FunctionPtr m_func_end;

    ClassPtr m_bases_beg;
    ClassPtr m_bases_end;

    //std::vector<Field> m_Fields;
    //std::vector<Function> m_Methods;
    //
    //std::vector<Class*> m_Bases;
    std::vector<Class*> m_Derived;
};