#pragma once

#include "Apex/Core/GUID.h"
#include "Apex/Core/Strings.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/FileSystem.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

// #include "ResourceLibrary.h"

#include "Apex/Graphics/Mesh.h"
#include "Apex/Graphics/Material.h"

#include <typeindex>
#include <variant>
#include <optional>

namespace Apex {
	
	using Handle = uint64_t;

#define RESNAME(_str_) static_cast<Handle>(HASH(_str_))

	enum class ResourceType : uint32_t
	{
		FILE	= 0,
		TEXTURE,
		SHADER,
		//SCENE,
		//SPRITE,
		//MODEL,
		MESH,
		MATERIAL,
		//ANIMATION,
		//AUDIO,

		SIZE
	};

	constexpr const char* ResourceTypeToString(ResourceType type)
	{
		switch (type) {
		case ResourceType::FILE:		return "FILE";
		case ResourceType::TEXTURE:		return "TEXTURE";
		case ResourceType::SHADER:		return "SHADER";
		//case ResourceType::SCENE:		return "SCENE";
		//case ResourceType::SPRITE:	return "SPRITE";
		//case ResourceType::MODEL:		return "MODEL";
		case ResourceType::MESH:		return "MESH";
		case ResourceType::MATERIAL:	return "MATERIAL";
		//case ResourceType::ANIMATION:	return "ANIMATION";
		//case ResourceType::AUDIO:		return "AUDIO";
		}
	};

#define RESOURCE_FN_DISTINCT(fn_name) \
	struct fn_name \
	{ \
		Resource& resource; \
		void operator() (const Ref<File>& file); \
		void operator() (const Ref<Texture>& texture); \
		void operator() (const Ref<Shader>& material); \
		void operator() (const Ref<Mesh>& mesh); \
		void operator() (const Ref<Material>& material); \
	}

	class Resource
	{
		using ResourcePtr_t = std::variant<
			Ref<File>,
			Ref<Texture>,
			Ref<Shader>,
			Ref<Mesh>,
			Ref<Material>
		>;

		struct _IsLoaded
		{
			template<typename ResourceType_t>
			bool operator() (const ResourceType_t& resource) { return resource == nullptr; }
		};
		
		RESOURCE_FN_DISTINCT(_LoadResource);
		RESOURCE_FN_DISTINCT(_ReloadResource);
		RESOURCE_FN_DISTINCT(_UnloadResource);

		// From: https://stackoverflow.com/a/31616949
		template<typename...> struct types { using type = types; };
		template<typename T>  struct tag   { using type = T; };
		template<typename Tag> using type_t = typename Tag::type;

	public:
		template<typename Resource_t>
		Resource(Handle id, const Ref<Resource_t>& ptr)
			: m_Id(id), m_Ptr(ptr)
		{
		}

		template<typename Resource_t>
		Resource(tag<Resource_t>, Handle id , const fs::path& filepath)
			: m_Id(id), m_Ptr(std::move(Ref<Resource_t>(nullptr))), m_SourceFile(filepath)
		{
		}

		~Resource() = default;

		[[nodiscard]] ResourceType GetType() const { return static_cast<ResourceType>(m_Ptr.index()); }
		[[nodiscard]] Handle GetId() const { return m_Id; }
		[[nodiscard]] fs::path GetSource() const { return m_SourceFile; }

		template<typename Resource_t>
		const Ref<Resource_t>& Get()
		{
			return std::get<Ref<Resource_t>>(m_Ptr);
		}

		template<typename Func>
		decltype(auto) Apply(Func&& func)
		{
			return std::visit(func, m_Ptr);
		}
		
		bool IsLoaded()
		{
			return std::visit(_IsLoaded{}, m_Ptr);
		}

	// protected:
		void Load()
		{
			std::visit(_LoadResource{ *this }, m_Ptr);
		}

		void Reload()
		{
			std::visit(_ReloadResource{ *this }, m_Ptr);
		}

		void Unload()
		{
			std::visit(_UnloadResource{ *this }, m_Ptr);
		}

	private:
		Handle m_Id;
		ResourcePtr_t m_Ptr;
		fs::path m_SourceFile;
		//std::vector<Resource*> m_Dependencies;
		//std::list<Resource*> m_Observers;
		bool m_IsLoaded = false;

		friend class ResourceManager;
	};

	using ResourceHandle_t = std::variant<Resource*, Handle>;

	inline auto GetResourceHandleFn = [](auto& texture) -> Handle {
		using T = std::decay_t<decltype(texture)>;
		if constexpr (std::is_same_v<Resource*, T>) {
			if (texture)
				return texture->GetId();
			else
				return 0;
		}
		else if constexpr (std::is_same_v<Handle, T>) {
			return texture;
		}
	};

#undef RESOURCE_FN_DISTINCT

	class ResourceManager
	{

	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		
		template<typename Resource_t>
		Resource& AddResource(Handle id, const Ref<Resource_t>& ptr)
		{
			APEX_CORE_ASSERT(!Exists(id), "Resource '" + TO_STRING(Strings::Get(id)) + "' already exists!");
			auto& [it, success] = m_Registry.try_emplace(id, id, ptr);
			return it->second;
		}

		template<typename Resource_t>
		Resource& AddResourceFromFile(Handle id, const std::string& filepath)
		{
			APEX_CORE_ASSERT(!Exists(id), "Resource '" + TO_STRING(Strings::Get(id)) + "' already exists!");
			auto& [it, success] = m_Registry.try_emplace(id, Resource::tag<Resource_t>{}, id, filepath);
			return it->second;
		}
		
		Resource* Get(Handle id);
		[[nodiscard]] const Resource* Get(Handle id) const;

		bool Exists(Handle id);

		// Dependency Graph

		void AddDependency(Handle dependent, Handle dependency);

		Iterable<std::vector<Handle>> SolveDependencies();

	protected:
		void TopologicalSort();

	private:
		std::unordered_map<Handle, Resource> m_Registry;
		std::unordered_map<Handle, std::list<Handle>> m_DependencyGraph;
		
		std::vector<Handle> m_SortedOrder;
		bool m_Unsorted = true; // similar to dirty bit

		friend class ResourceSerializer;
	};

}
