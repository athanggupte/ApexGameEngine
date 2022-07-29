#include "Class.h"
#include "Type.h"
#include "Field.h"
#include "Function.h"
#include <vector>

//// Class ////
//const std::vector<Field>& Class::Fields() const
//{
//    return m_Fields;
//}
//
//const std::vector<Function>& Class::Methods() const
//{
//    return m_Methods;
//}
//
//Field& Class::AddField()
//{
//    Field& newField = m_Fields.emplace_back();
//    newField.m_ParentClass = this;
//    return newField;
//}
//
//Function& Class::AddMethod(const detail::FunctionImplBase* impl)
//{
//    Function& newMethod = m_Methods.emplace_back(impl);
//    // newMethod.m_Impl->m_ParentClass = this;
//    return newMethod;
//}

bool Class::IsBaseOf(Class* other) const
{
    return std::find(other->m_bases_beg, other->m_bases_end, this);
}

void Class::_InitializeStorage(FieldPtr field_beg, FieldPtr field_end, FunctionPtr func_beg, FunctionPtr func_end, ClassPtr bases_beg, ClassPtr bases_end)
{
    m_field_beg = field_beg; m_field_end = field_end;
    m_func_beg  = func_beg;  m_func_end  = func_end;
    m_bases_beg = bases_beg; m_bases_end = bases_end;

    for (auto ptr = m_bases_beg; ptr != m_bases_end; ++ptr) {
        (*ptr)->m_Derived.push_back(this);
    }
}

bool Class::_Convert(const Any& from, Any& to) const
{
    //if (this == to.Type())
        return false;
    // return detail::DefaultTypeConverter<T>::ConvertTo(from.Cast<T>(), to);
}