#define SKIP_TYPE_DECLARATIONS
#include "Type.h"
#include "Any.h"

//// Type ////
size_t Type::Size() const
{
    return m_Size;
}

// Qualifiers
bool Type::IsReference() const
{
    return m_QualifierFlags & Reflect::Qualifiers::eReference;
}

bool Type::IsPointer() const
{
    return m_QualifierFlags & Reflect::Qualifiers::ePointer;
}

bool Type::IsArray() const
{
    return m_QualifierFlags & Reflect::Qualifiers::eArray;
}

bool Type::IsConst() const
{
    return m_QualifierFlags & Reflect::Qualifiers::eConst;
}

bool Type::IsContainer() const
{
    return m_QualifierFlags & Reflect::Qualifiers::eTemplate;
}

// Type info
bool Type::IsFundamental() const
{
    return IsArithmetic() || IsVoid() || IsNullptr();
}

bool Type::IsVoid() const
{
    return GetTypeInfo() == Reflect::TypeInfo::eVoid;
}

bool Type::IsNullptr() const
{
    return GetTypeInfo() == Reflect::TypeInfo::eNullptr;
}

bool Type::IsArithmetic() const
{
    return IsIntegral() || IsFloatingPoint();
}

bool Type::IsIntegral() const
{
    return GetTypeInfo() == Reflect::TypeInfo::eIntegral;
}

bool Type::IsFloatingPoint() const
{
    return GetTypeInfo() == Reflect::TypeInfo::eFloatingPoint;
}

bool Type::IsCompound() const
{
    return !IsFundamental();
}

bool Type::IsClass() const
{
    return GetTypeInfo() == Reflect::TypeInfo::eClass;
}

bool Type::IsEnum() const
{
    return GetTypeInfo() == Reflect::TypeInfo::eEnum;
}

bool Type::IsUnsigned() const
{
    return m_IsUnsigned;
}


const std::vector<Type*>& Type::GetTemplateParamTypes() const
{
    return m_TemplateParamTypes;
}

