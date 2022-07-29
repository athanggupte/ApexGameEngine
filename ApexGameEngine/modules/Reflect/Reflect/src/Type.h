#pragma once
#include <vector>
#include <string>

#include "config.h"
#include "TypeTraits.h"

#pragma warning(disable : 4251)

class Any;

namespace Reflect {
    
    enum TypeInfo : uint16_t
    {
        eVoid = 0,
        eNullptr,

        eIntegral,
        eFloatingPoint,
        eString,
        eClass,
        eEnum,
        eUnion,

        eUniquePointer,
        eSharedPointer,
        // eWeakPointer,

        eVector,
        eMatrix,

        eList,
        ePair,
        eMap,
        eTuple,
    };

    enum Qualifiers : uint16_t
    {
        eNone = 0,
        eReference = 1 << 0,
        ePointer = 1 << 1,
        eConst = 1 << 2,
        eArray = 1 << 3,
        eTemplate = 1 << 4,
    };
}

struct REFLECT_API Type
{
// Type-info accessors
    virtual std::string_view Name() const = 0;
    size_t Size() const;

    bool IsReference() const;
    bool IsPointer() const;
    bool IsArray() const;
    bool IsConst() const;
    bool IsContainer() const;

    //virtual void* Construct() const;
    
    // Type info
    bool IsFundamental() const;
    bool IsVoid() const;
    bool IsNullptr() const;
    bool IsArithmetic() const;
    bool IsIntegral() const;
    bool IsFloatingPoint() const;
    
    bool IsCompound() const;
    bool IsClass() const;
    bool IsEnum() const;

    bool IsUnsigned() const;

    const std::vector<Type*>& GetTemplateParamTypes() const;

    template <typename T>
    std::enable_if_t<std::is_base_of_v<Type, T>, T*>
    CastAs()
    {
        return dynamic_cast<T*>(this);
    }

// Members
    // const char* m_Name = nullptr;
    size_t m_Size = 0;
    bool m_IsUnsigned = false;
    std::vector<Type*> m_TemplateParamTypes; // Set for Container types

    virtual Reflect::TypeInfo GetTypeInfo() const = 0;

    using QualifierFlags = uint16_t;
    QualifierFlags m_QualifierFlags = 0;

    struct IMemoryFunctions
    {
        virtual void _DestroyPtr(void* target) const = 0;
        virtual void* _CopyPtr(const void* source) const = 0;
    };

    // TypeFns* m_TypeFns;
    virtual const IMemoryFunctions* _GetMemoryFunctions() const = 0;

protected:
    virtual bool _Convert(const Any& from, Any& to) const = 0;

    friend struct Class;
    friend struct Enum;
    friend class Any;
};

struct Void {};

//#include "Class.h"
//#include "Enum.h"

template <typename T>
Class* GetClass();

template <typename T>
Type* GetType();

template <typename T>
Class* GetClass(T obj)
{
    return GetClass<T>();
}

template <typename T>
Type* GetType(T obj)
{
    return GetType<T>();
}

class TypeRegistry;

REFLECT_API Type* GetType(uint64_t id);
REFLECT_API TypeRegistry& GetTypeRegistry();

template <typename T> struct ClassImpl;
template <typename T> struct TypeImpl;

//#ifndef SKIP_TYPE_DECLARATIONS
//#include "TypeImpl.hpp"
//#endif