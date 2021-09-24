#pragma once

#include "Apex/Core/GUID.h"
#include "Apex/Core/Strings.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/FileHandle.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

// #include "ResourceLibrary.h"

#include <typeindex>
#include <variant>
#include <optional>

namespace Apex {
	
	using Handle = uint64_t;
	
	enum class ResourceType : size_t
	{
		FILE	= 0,
		TEXTURE,
		SHADER,
		//MESH,
		//AUDIO,
		//ANIMATION,
		//SCENE,

		SIZE
	};

	constexpr const char* ResourceTypeToString(ResourceType type)
	{
		switch (type) {
		case ResourceType::FILE:		return "FILE";
		case ResourceType::TEXTURE:		return "TEXTURE";
		case ResourceType::SHADER:		return "SHADER";
		//case ResourceType::MESH:		return "MESH";
		//case ResourceType::AUDIO:		return "AUDIO";
		//case ResourceType::ANIMATION:	return "ANIMATION";
		}
	};

#define RESOURCE_FN_DISTINCT(fn_name) \
	struct fn_name \
	{ \
		Resource& resource; \
		void operator() (const Ref<VFS::IFile>& texture); \
		void operator() (const Ref<Texture>& texture); \
		void operator() (const Ref<Shader>& texture); \
	}

	class Resource
	{
		using ResourcePtr_t = std::variant<Ref<VFS::IFile>, Ref<Texture>, Ref<Shader>>;

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
		Resource(tag<Resource_t>, Handle id , StringHandle filepath)
			: m_Id(id), m_Ptr(std::move(Ref<Resource_t>(nullptr))), m_SourceFile(filepath)
		{
		}

		~Resource() = default;

		ResourceType GetType() const { return (ResourceType)m_Ptr.index(); }
		Handle GetId() const { return m_Id; }
		StringHandle GetSource() const { return m_SourceFile; }

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
		StringHandle m_SourceFile;
		//std::vector<Resource*> m_Dependencies;
		//std::list<Resource*> m_Observers;
		bool m_IsLoaded = false;

		friend class ResourceManager;
	};

	using ResourceHandle_t = std::variant<Resource*, Handle>;
	
	auto GetResourceHandleFn = [](auto& texture) -> Handle {
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

		template<typename Resource_t, typename... Args>
		Resource& AddResource(Handle id, Args&&... args)
		{
			APEX_CORE_ASSERT(!Exists(id), "Resource '" + TO_STRING(Strings::Get(id)) + "' already exists!");
			auto& [it, success] = m_Registry.emplace(id, Resource(Resource::tag<Resource_t>{}, id, std::forward<Args>(args)...));
			return it->second;
		}
		
		Resource* Get(Handle id);
		const Resource* const Get(Handle id) const;

		//template<typename Resource_t>
		//const Ref<Resource_t>& GetResource(Handle id)
		//{
		//	auto itr = m_Registry.find(id);
		//	return std::get<Ref<Resource_t>>(itr->second);
		//}

		inline bool Exists(Handle id)
		{
			return m_Registry.find(id) != m_Registry.end();
		}

		// Dependency Graph

		void AddDependency(Handle dependent, Handle dependency)
		{
			auto it = m_DependencyGraph.find(dependent);
			if (it != m_DependencyGraph.end()) {
				it->second.push_back(dependency);
			}
			else {
				m_DependencyGraph.emplace(dependent, std::list<Handle>{ dependency });
			}
		}
		
		std::vector<Handle> SolveDependencies()
		{
			return TopologicalSort();
		}

	protected:
		std::vector<Handle> TopologicalSort();

	private:
		std::unordered_map<Handle, Resource> m_Registry;
		std::unordered_map<Handle, std::list<Handle>> m_DependencyGraph;
		//std::vector<Handle> m_SortedOrder;

		friend class ResourceSerializer;
	};

}
