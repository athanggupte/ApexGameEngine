#include "Registry.h"
#include "Type.h"
#include "Class.h"
#include "Enum.h"
#include <cassert>
#include <iostream>

TypeRegistry::TypeRegistry(size_t base_size)
{
	m_ClassRegistry.reserve(base_size);
	m_EnumRegistry.reserve(base_size);
}

void TypeRegistry::AddClass(uint64_t id, Class* _class)
{
	assert(m_ClassRegistry.find(id) == m_ClassRegistry.end());
	m_ClassRegistry.emplace(id, _class);
}

void TypeRegistry::AddEnum(uint64_t id, Enum* _enum)
{
	assert(m_EnumRegistry.find(id) == m_EnumRegistry.end());
	m_EnumRegistry.emplace(id, _enum);
}

Class* TypeRegistry::GetClass(uint64_t id)
{
	auto itr = m_ClassRegistry.find(id);
	if (itr != m_ClassRegistry.end())
		return itr->second;
	return nullptr;
}

Enum* TypeRegistry::GetEnum(uint64_t id)
{
	auto itr = m_EnumRegistry.find(id);
	if (itr != m_EnumRegistry.end())
		return itr->second;
	return nullptr;
}

Type* TypeRegistry::GetType(uint64_t id)
{
	auto _class = GetClass(id);
	if (_class) return _class;

	auto _enum = GetEnum(id);
	if (_enum) return _enum;

	return nullptr;
}

REFLECT_API Type* GetType(uint64_t id)
{
	return GetTypeRegistry().GetType(id);
}

REFLECT_API TypeRegistry& GetTypeRegistry()
{
	static TypeRegistry s_TypeRegistry(50);
	return s_TypeRegistry;
}
