#pragma once
#include "Type.h"
#include "Any.h"
#include "Class.h"
#include "Enum.h"
#include "ClassStorage.h"
#include "TypeTraits.h"
#include "Registry.h"
#include "TypeConverter.h"

//#include "Util/HashFNV1a.h"

// TODO: Remove these includes
#include "glm/glm.hpp"

#include <iostream>

template <typename T> struct TypeTag
{
    explicit TypeTag() = default;
};

template <typename T>
Class* GetClassImpl(TypeTag<T>);

template <typename T>
Class* GetClass()
{
    return GetClassImpl(TypeTag<T>{});
}

template <typename T>
Enum* GetEnumImpl(TypeTag<T>);

template <typename T>
Enum* GetEnum()
{
    return GetEnumImpl(TypeTag<T>{});
}

template <typename T>
Type* GetTypeImpl(TypeTag<T>);

template <typename T>
Type* GetType()
{
    return GetTypeImpl(TypeTag<T>{});
}

template <typename T>
struct MemoryFunctionsImpl : public Type::IMemoryFunctions
{
    void _DestroyPtr(void* target) const override
    {
        delete static_cast<T*>(target);
    }
    
    void* _CopyPtr(const void* source) const override
    {
        return new T(*static_cast<const T*>(source));
    }
};

template <>
struct MemoryFunctionsImpl<void> : public Type::IMemoryFunctions
{
    void _DestroyPtr(void* /*target*/) const override
    {
        // Cannot destroy void. Nothing to do.
    }
    
    void* _CopyPtr(const void* /*source*/) const override
    {
        // Cannot copy void. Nothing to do.
        return nullptr;
    }
};

template <typename T>
struct _TypeImpl : public Type, public MemoryFunctionsImpl<T>
{
    _TypeImpl()
    {
        m_Size = sizeof(T);
        m_IsUnsigned = std::is_unsigned_v<T>;
    }

    const IMemoryFunctions* _GetMemoryFunctions() const override
    {
        return this;
    }
    
    bool _Convert(const Any& from, Any& to) const override
    {
        return detail::DefaultTypeConverter<T>::ConvertTo(from.Cast<T>(), to);
    }
};

template <typename T>
struct _ClassImpl : public Class, public MemoryFunctionsImpl<T>
{
    _ClassImpl()
    {
        m_Size = sizeof(T);
    }

    template <size_t NumBases, size_t NumFields, size_t NumFuncs, size_t NumTemplateArgs = 0>
    void _InitializeStorage(detail::ClassStorage<T, NumBases, NumFields, NumFuncs, NumTemplateArgs>& class_storage)
    {
        this->Class::_InitializeStorage(class_storage.fields.data(), class_storage.fields.data() + NumFields,
            class_storage.functions.data(), class_storage.functions.data() + NumFuncs,
            class_storage.bases.data(), class_storage.bases.data() + NumBases);
    }

    const IMemoryFunctions* _GetMemoryFunctions() const override
    {
        return this;
    }
};

template <typename T>
struct _EnumImpl : public Enum, public MemoryFunctionsImpl<T>
{
    _EnumImpl()
    {
        m_Size = sizeof(T);
    }

    Type* GetUnderlyingType() const override
    {
        return GetType<std::underlying_type_t<T>>();
    }

    const IMemoryFunctions* _GetMemoryFunctions() const override
    {
        return this;
    }
};

template <typename T>
struct TypeImpl : public _TypeImpl<T>
{
    using BaseType = full_decay_t<T>;
};

template <typename T>
struct ClassImpl : public _ClassImpl<T>
{
    using BaseType = full_decay_t<T>;
};

template <typename T>
struct EnumImpl : public _EnumImpl<T>
{
    using BaseType = full_decay_t<T>;
};

namespace detail {
    template <typename T>
    Type* ApplyQualifiers(Type* type)
    {
        if constexpr (std::is_const_v<T>) {
            type->m_QualifierFlags |= Reflect::Qualifiers::eConst;
            //std::cout << "isConst" << '\n';
        }
        if constexpr (std::is_reference_v<T>) {
            type->m_QualifierFlags |= Reflect::Qualifiers::eReference;
            if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
                type->m_QualifierFlags |= Reflect::Qualifiers::eConst;
            }
            //std::cout << "isReference" << '\n';
        }
        if constexpr (std::is_array_v<T>) {
            type->m_QualifierFlags |= Reflect::Qualifiers::eArray;
            type->m_Size = sizeof(T);
            //std::cout << "isArray" << '\n';
        }
        if constexpr (std::is_pointer_v<T>) {
            type->m_QualifierFlags |= Reflect::Qualifiers::ePointer;
            type->m_Size = sizeof(T);
            //std::cout << "isPointer" << '\n';
        }
        return type;
    }
}

template <typename T>
Class* GetClassImpl(TypeTag<T>)
{
    using BaseType = full_decay_t<T>;
    if constexpr (std::is_pointer_v<T>) {
        using DecayedType = std::remove_pointer_t<T>;
        static ClassImpl<BaseType> type = *static_cast<ClassImpl<BaseType>*>(GetClassImpl(TypeTag<DecayedType>{}));
        detail::ApplyQualifiers<T>(&type);
        return &type;
    } else if constexpr (std::is_array_v<T>) {
        using DecayedType = std::remove_pointer_t<std::decay_t<T>>;
        static ClassImpl<BaseType> type = *static_cast<ClassImpl<BaseType>*>(GetClassImpl(TypeTag<DecayedType>{}));
        detail::ApplyQualifiers<T>(&type);
        return &type;
    } else {
        using DecayedType = std::decay_t<T>;
        static ClassImpl<BaseType> type = *static_cast<ClassImpl<BaseType>*>(GetClassImpl(TypeTag<DecayedType>{}));
        detail::ApplyQualifiers<T>(&type);
        return &type;
    }
}

template <typename T>
Enum* GetEnumImpl(TypeTag<T>)
{
    using BaseType = full_decay_t<T>;
    if constexpr (std::is_pointer_v<T>) {
        using DecayedType = std::remove_pointer_t<T>;
        static EnumImpl<BaseType> type = *static_cast<EnumImpl<BaseType>*>(GetEnumImpl(TypeTag<DecayedType>{}));
        detail::ApplyQualifiers<T>(&type);
        return &type;
    } else if constexpr (std::is_array_v<T>) {
        using DecayedType = std::remove_pointer_t<std::decay_t<T>>;
        static EnumImpl<BaseType> type = *static_cast<EnumImpl<BaseType>*>(GetEnumImpl(TypeTag<DecayedType>{}));
        detail::ApplyQualifiers<T>(&type);
        return &type;
    } else {
        using DecayedType = std::decay_t<T>;
        static EnumImpl<BaseType> type = *static_cast<EnumImpl<BaseType>*>(GetEnumImpl(TypeTag<DecayedType>{}));
        detail::ApplyQualifiers<T>(&type);
        return &type;
    }
}

template <typename T>
Type* GetTypeImpl(TypeTag<T>)
{
    using BaseType = full_decay_t<T>;
    if constexpr (IsBuiltInType<BaseType>::value) {
        if constexpr (std::is_pointer_v<T>) {
            using DecayedType = std::remove_pointer_t<T>;
            static TypeImpl<BaseType> type = *static_cast<TypeImpl<BaseType>*>(GetTypeImpl(TypeTag<DecayedType>{}));
            return detail::ApplyQualifiers<T>(&type);
        } else if constexpr (std::is_array_v<T>) {
            using DecayedType = std::remove_pointer_t<std::decay_t<T>>;
            static TypeImpl<BaseType> type = *static_cast<TypeImpl<BaseType>*>(GetTypeImpl(TypeTag<DecayedType>{}));
            return detail::ApplyQualifiers<T>(&type);
        } else {
            using DecayedType = std::decay_t<T>;
            static TypeImpl<BaseType> type = *static_cast<TypeImpl<BaseType>*>(GetTypeImpl(TypeTag<DecayedType>{}));
            return detail::ApplyQualifiers<T>(&type);
        }
    } else if  constexpr(std::is_enum_v<T>) {
        return GetEnum<T>();
    } else {
        return GetClass<T>();
    }
    // return nullptr;
}

#define STR(x) #x
#define XSTR(x) STR(x)

#define DEFINE_TYPE_NAME(_type_) \
    std::string_view Name() const override \
    { \
        constexpr static std::string_view name = STR(_type_); \
        return name; \
    }

//#define REGISTER_CLASS(_type_) \
//    GetTypeRegistry().AddClass(static_cast<uint64_t>(TEXT(STR(_type_))), this)
//
//#define REGISTER_ENUM(_type_) \
//    GetTypeRegistry().AddEnum(static_cast<uint64_t>(TEXT(STR(_type_))), this)

#define DEFINE_ENUM_NAME_OF(_enum_) \
	virtual std::string_view NameOf(const Any& value) override \
	{ \
		auto& val = value.Cast<_enum_>(); \
		if constexpr (is_contiguous) { \
			return NAMES[(size_t)val - (size_t)VALUES[0]]; \
		} else { \
			if (auto ptr = std::find(std::begin(VALUES), std::end(VALUES), val); std::end(VALUES) != ptr) { \
				auto idx = ptr - std::begin(VALUES); \
				return NAMES[idx]; \
			} \
		} \
		return ""; \
	}

// #define DEFINE_ENUM

#ifdef TYPE_IMPL_CPP
	#define DEFINE_TYPE_IMPL(_type_, _type_info_) \
        template <> \
	    struct TypeImpl<_type_> : public _TypeImpl<_type_> \
	    { \
	        TypeImpl() \
	        { \
	            /*std::cout << "[INIT] :: TypeImpl < " STR(_type_) " >" << '\n';*/ \
	            /*REGISTER_TYPE(_type_);*/ \
	        } \
	        Reflect::TypeInfo GetTypeInfo() const override \
	        { \
	            return _type_info_; \
	        } \
	        DEFINE_TYPE_NAME(_type_) \
	    };

	#define DEFINE_GET_TYPE_IMPL(_type_) \
	    template <> \
	    Type* GetTypeImpl(TypeTag<_type_>) \
	    { \
	        static TypeImpl<_type_> type; \
	        return &type;\
	    }\
	    template Type* GetTypeImpl(TypeTag<_type_>);

	#define DEFINE_GET_TYPE(_type_) \
	    template REFLECT_DLL_EXPORT Type* GetType<_type_>();
#else
	#define DEFINE_TYPE_IMPL(...)
	#define DEFINE_GET_TYPE_IMPL(...)
	// extern template REFLECT_DLL_IMPORT Type* GetTypeImpl(TypeTag<_type_>);
	#define DEFINE_GET_TYPE(_type_) \
	    extern template Type* GetType<_type_>();
#endif


#define DEFINE_TYPE(_type_, _type_info_) \
    DEFINE_TYPE_IMPL(_type_,  _type_info_) \
    DEFINE_GET_TYPE_IMPL(_type_) \
    DEFINE_GET_TYPE(_type_)

template <>
struct TypeImpl<void> : public Type, public MemoryFunctionsImpl<void>
{
    TypeImpl()
    {
        m_Size = 0;
        //REGISTER_TYPE(void);
    }

    Reflect::TypeInfo GetTypeInfo() const override
    {
        return Reflect::TypeInfo::eVoid;
    }

    const IMemoryFunctions* _GetMemoryFunctions() const override
    {
        return this;
    }

    bool _Convert(const Any& from, Any& to) const override
    {
        return false;
    }

    DEFINE_TYPE_NAME(void)
};

#ifdef TYPE_IMPL_CPP
template <>
REFLECT_API Type* GetType<void>()
{
    static TypeImpl<void> type;
    return &type;
}
#endif

DEFINE_GET_TYPE(void)
// template Type* GetType<void>();

#ifdef TYPE_IMPL_CPP
template <>
Type* GetType<Void>()
{
    return GetType<void>();
}
#endif

DEFINE_GET_TYPE(Void)
// template Type* GetType<Void>();


//DEFINE_TYPE(std::nullptr_t, Reflect::TypeInfo::eNullptr)

// Integral types
DEFINE_TYPE(bool, Reflect::TypeInfo::eIntegral)

#ifndef USE_SIZED_TYPENAMES
DEFINE_TYPE(char, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(short, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(int, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(long long, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(unsigned char, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(unsigned short, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(unsigned int, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(unsigned long long, Reflect::TypeInfo::eIntegral)
#else
DEFINE_TYPE(int8_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(int16_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(int32_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(int64_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(uint8_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(uint16_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(uint32_t, Reflect::TypeInfo::eIntegral)
DEFINE_TYPE(uint64_t, Reflect::TypeInfo::eIntegral)
#endif

// Floating types
DEFINE_TYPE(float, Reflect::TypeInfo::eFloatingPoint)
DEFINE_TYPE(double, Reflect::TypeInfo::eFloatingPoint)

// String types
DEFINE_TYPE(std::string, Reflect::TypeInfo::eString)
DEFINE_TYPE(std::wstring, Reflect::TypeInfo::eString)

// Vector types
DEFINE_TYPE(glm::vec2, Reflect::TypeInfo::eVector)
DEFINE_TYPE(glm::vec3, Reflect::TypeInfo::eVector)
DEFINE_TYPE(glm::vec4, Reflect::TypeInfo::eVector)

DEFINE_TYPE(glm::ivec2, Reflect::TypeInfo::eVector)
DEFINE_TYPE(glm::ivec3, Reflect::TypeInfo::eVector)
DEFINE_TYPE(glm::ivec4, Reflect::TypeInfo::eVector)

// Matrix types
DEFINE_TYPE(glm::mat2, Reflect::TypeInfo::eMatrix)
DEFINE_TYPE(glm::mat3, Reflect::TypeInfo::eMatrix)
DEFINE_TYPE(glm::mat4, Reflect::TypeInfo::eMatrix)

// Smart Pointers
/*////////////////////////////////////////////////
    unique_ptr
////////////////////////////////////////////////*/
template <typename T>
struct TypeImpl<std::unique_ptr<T>> : public _TypeImpl<std::unique_ptr<T>>
{
    TypeImpl()
    {
        m_Size = sizeof(std::unique_ptr<T>);
        m_QualifierFlags = Reflect::Qualifiers::eTemplate;
        m_TemplateParamTypes.push_back(GetType<T>());
    }

    Reflect::TypeInfo GetTypeInfo() const
    {
        return Reflect::TypeInfo::eUniquePointer;
    }

    DEFINE_TYPE_NAME(std::unique_ptr)
};

template <typename T>
Type* GetTypeImpl(TypeTag<std::unique_ptr<T>>)
{
    static TypeImpl<std::unique_ptr<T>> type;
    return &type;
}

/*////////////////////////////////////////////////
    shared_ptr
////////////////////////////////////////////////*/
template <typename T>
struct TypeImpl<std::shared_ptr<T>> : public _TypeImpl<std::shared_ptr<T>>
{
    TypeImpl()
    {
        m_Size = sizeof(std::shared_ptr<T>);
        m_QualifierFlags = Reflect::Qualifiers::eTemplate;
        m_TemplateParamTypes.push_back(GetType<T>());
    }

    Reflect::TypeInfo GetTypeInfo() const
    {
        return Reflect::TypeInfo::eSharedPointer;
    }

    DEFINE_TYPE_NAME(std::shared_ptr)
};

template <typename T>
Type* GetTypeImpl(TypeTag<std::shared_ptr<T>>)
{
    static TypeImpl<std::shared_ptr<T>> type;
    return &type;
}

// Standard Library Containers
/*////////////////////////////////////////////////
    vector
////////////////////////////////////////////////*/
template <typename T>
struct TypeImpl<std::vector<T>> : public _TypeImpl<std::vector<T>>
{
    TypeImpl()
    {
        //REGISTER_TYPE(std::vector);
        m_Size = sizeof(std::vector<T>);
        m_QualifierFlags = Reflect::Qualifiers::eTemplate;
        m_TemplateParamTypes.push_back(GetType<T>());
    }

    Reflect::TypeInfo GetTypeInfo() const
    {
        return Reflect::TypeInfo::eList;
    }

    DEFINE_TYPE_NAME(std::vector)
};

template <typename T>
Type* GetTypeImpl(TypeTag<std::vector<T>>)
{
    static TypeImpl<std::vector<T>> type;
    return &type;
}

/*////////////////////////////////////////////////
    pair
////////////////////////////////////////////////*/
template <typename T, typename U>
struct TypeImpl<std::pair<T, U>> : public _TypeImpl<std::pair<T, U>>
{
    TypeImpl()
    {
        //REGISTER_TYPE(std::pair);
        m_Size = sizeof(std::pair<T, U>);
        m_QualifierFlags = Reflect::Qualifiers::eTemplate;
        m_TemplateParamTypes.push_back(GetType<T>());
        m_TemplateParamTypes.push_back(GetType<U>());
    }

    Reflect::TypeInfo GetTypeInfo() const override
    {
        return Reflect::TypeInfo::ePair;
    }

    DEFINE_TYPE_NAME(std::pair)
};

template <typename T, typename U>
Type* GetTypeImpl(TypeTag<std::pair<T, U>>)
{
    static TypeImpl<std::pair<T,U>> type;
    return &type;
}
