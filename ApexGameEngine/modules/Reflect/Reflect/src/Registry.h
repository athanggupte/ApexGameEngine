#pragma once

#include "config.h"
#include <unordered_map>
//#include <entt/entt.hpp>

struct Type;
struct Class;
struct Enum;

class REFLECT_API TypeRegistry
{
public:
	TypeRegistry(size_t base_size);

	void AddClass(uint64_t id, Class* _class);
	void AddEnum(uint64_t id, Enum* _enum);
	Type* GetType(uint64_t id);
	Class* GetClass(uint64_t id);
	Enum* GetEnum(uint64_t id);

	template <typename Func>
	void VisitClasses(Func&& func)
	{
		for (auto [id, _class] : m_ClassRegistry) {
			func(_class);
		}
	}

	template <typename Func>
	void VisitEnums(Func&& func)
	{
		for (auto [id, _enum] : m_EnumRegistry) {
			func(_enum);
		}
	}

private:
	std::unordered_map<uint64_t, Class*> m_ClassRegistry;
	std::unordered_map<uint64_t, Enum*> m_EnumRegistry;
};

//class MetaManager
//{
//public:
//	template <typename Property, typename... Args>
//	decltype(auto) AddMetaProperty(const Field& field, Args&&... args)
//	{
//		return m_Registry.emplace<Property>(field.id, std::forward<Args>(args)...);
//	}
//
//	template <typename Property>
//	decltype(auto) GetMetaProperty(const Field& field)
//	{
//		
//	}
//
//private:
//	MetaManager() = default;
//
//private:
//	entt::registry m_Registry;
//};


