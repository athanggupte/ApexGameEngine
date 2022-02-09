#pragma once

#include "Apex/Core/Strings.h"
#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/FileSystem.h"

// #include "ResourceLibrary.h"

namespace Apex {
	class AScriptFactory;
	class Shader;
	class Texture;
	class Mesh;
	class Material;

	using Handle = uint64_t;

#define RESNAME(_str_) static_cast<Apex::Handle>(HASH(_str_))

	enum class ResourceType : uint32_t
	{
		NONE = 0,
		FILE,
		TEXTURE,
		SHADER,
		SCRIPT,
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
		case ResourceType::SCRIPT:		return "SCRIPT";
		//case ResourceType::SCENE:		return "SCENE";
		//case ResourceType::SPRITE:	return "SPRITE";
		//case ResourceType::MODEL:		return "MODEL";
		case ResourceType::MESH:		return "MESH";
		case ResourceType::MATERIAL:	return "MATERIAL";
		//case ResourceType::ANIMATION:	return "ANIMATION";
		//case ResourceType::AUDIO:		return "AUDIO";
		case ResourceType::NONE: break;
		case ResourceType::SIZE: break;
		}
		return "UNKNOWN";
	};

	template<typename Resource_t>
	constexpr ResourceType GetResourceType()
	{
		if constexpr (std::is_same_v<Resource_t, File>)				return ResourceType::FILE;
		if constexpr (std::is_same_v<Resource_t, Texture>)			return ResourceType::TEXTURE;
		if constexpr (std::is_same_v<Resource_t, Shader>)			return ResourceType::SHADER;
		if constexpr (std::is_same_v<Resource_t, AScriptFactory>)	return ResourceType::SCRIPT;
		if constexpr (std::is_same_v<Resource_t, Mesh>)				return ResourceType::MESH;
		if constexpr (std::is_same_v<Resource_t, Material>)			return ResourceType::MATERIAL;
		else return ResourceType::NONE;
	}

	class ResourceManager;

	template<typename Resource_t>
	class Resource
	{
	private:
		Resource(Handle id, Ref<Resource_t>* ptr/*, size_t index*/)
			: m_Id(id), m_Ptr(ptr)/*, m_Index(index)*/
		{
		}

	public:
		Resource()
		{
		}

		~Resource() = default;

		//Resource(const Resource&) = default;
		//Resource(Resource&&) = default;
		
		[[nodiscard]] Handle GetId() const { return m_Id; }

		Resource_t* operator -> () { return (*m_Ptr).template operator-><Resource_t>(); }
		Resource_t& operator * () { return (*m_Ptr).template operator*<Resource_t>(); }

		const Resource_t* operator -> () const { return (*m_Ptr).template operator-><Resource_t>(); }
		const Resource_t& operator * () const { return (*m_Ptr).template operator-><Resource_t>(); }

		[[nodiscard]] auto& Get() { return *m_Ptr; }
		[[nodiscard]] const auto& Get() const { return *m_Ptr; }

		[[nodiscard]] bool IsNull() const
		{
			return m_Id == 0 || m_Ptr == nullptr;
		}

		[[nodiscard]] bool IsValid() const
		{
			return ! IsNull();
		}

		[[nodiscard]] bool IsLoaded() const
		{
			APEX_CORE_ASSERT(IsValid(), "Resource is not validated!");
			return static_cast<bool>(*m_Ptr);
		}

	private:
		Handle m_Id = 0;
		Ref<Resource_t>* m_Ptr = nullptr;
		//size_t m_Index = -1;

		friend class ResourceManager;
	};

	class APEX_API ResourceManager
	{
		template<typename T>
		struct PoolItem
		{
			Handle first;
			Ref<T> second;

			[[nodiscard]] Resource<T> ToResource()
			{
				return Resource<T>(first, &second);
			}
		};

		//template<typename T> using Pool = BucketList<std::pair<Handle, Ref<T>>>;
		template<typename T> using Pool = BucketList<PoolItem<T>>;

	public:
		struct ResourceData
		{
			ResourceType type;
			fs::path sourceFile;
			// std::list<Handle> dependencies;
		};

	private:
		struct IndexData
		{
			size_t index;
			ResourceData resourceData;

			IndexData(size_t index, ResourceType type, const fs::path& sourceFile)
				: index(index), resourceData{ type, sourceFile }
			{
			}

		};

	private:
		ResourceManager(const ResourceManager& other);

		// using IndexData = std::pair<size_t, ResourceData>;
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		
		template<typename Resource_t>
		Resource<Resource_t> Insert(Handle id, const Ref<Resource_t>& ptr)
		{
			APEX_CORE_ASSERT(!Exists(id), "Resource '" + TO_STRING(Strings::Get(id)) + "' already exists!");
			auto& resourcePool = GetPoolToUse<Resource_t>();
			size_t index = resourcePool.size();
			auto& [it, success] = m_Registry.try_emplace(id, index, GetResourceType<Resource_t>(), "");
			APEX_CORE_ASSERT(success, "Could not add resource!");
			resourcePool.push_back({ id, ptr });
			return Resource<Resource_t>{ id, &resourcePool.back().second/*, index*/ };
			// resourcePool.push_back(Resource<Resource_t>{ id, ptr, index, this });
			// return resourcePool.back();
		}

		template<typename Resource_t>
		Resource<Resource_t> Insert(Handle id, const fs::path& filepath)
		{
			APEX_CORE_ASSERT(!Exists(id), "Resource '" + TO_STRING(Strings::Get(id)) + "' already exists!");
			auto& resourcePool = GetPoolToUse<Resource_t>();
			size_t index = resourcePool.size();
			auto& [it, success] = m_Registry.try_emplace(id, index, GetResourceType<Resource_t>(), filepath.string());
			APEX_CORE_ASSERT(success, "Could not add resource!");
			resourcePool.push_back({ id, nullptr });
			return Resource<Resource_t>{ id, &resourcePool.back().second/*, index*/ };
			// resourcePool.push_back(Resource<Resource_t>{ id, nullptr, index, this });
			// return resourcePool.back();
		}

		template<typename Resource_t>
		Resource<Resource_t> Insert(const fs::path& filepath)
		{
			Handle id = RESNAME(filepath.filename().string());
			return Insert<Resource_t>(id, filepath);
		}

		template<typename Resource_t>
		[[nodiscard]] Resource<Resource_t> Get(Handle id)
		{
			const auto itr = m_Registry.find(id);
			APEX_CORE_ASSERT(itr != m_Registry.end(), fmt::format("Resource '{}' not found!", Strings::Get(id)));
			auto& resourcePool = GetPoolToUse<Resource_t>();
			if (itr != m_Registry.end()) {
				APEX_CORE_ASSERT(itr->second.resourceData.type == GetResourceType<Resource_t>(),
				                 fmt::format("Invalid resource types! Expected '{0}', got '{1}'!",
									 ResourceTypeToString(GetResourceType<Resource_t>()), itr->second.resourceData.type));
				return Resource<Resource_t>{ id, &resourcePool[itr->second.index].second/*, itr->second.index*/ };
			}
			return Resource<Resource_t>{};
		}

		template<typename Resource_t>
		[[nodiscard]] Resource<Resource_t> GetOrEmplace(Handle id, const Ref<Resource_t>& ptr)
		{
			return Exists(id) ? Get<Resource_t>(id) : Insert<Resource_t>(id, ptr);
		}

		template<typename Resource_t>
		[[nodiscard]] Resource<Resource_t> GetOrEmplace(Handle id, const fs::path& filepath)
		{
			return Exists(id) ? Get<Resource_t>(id) : Insert<Resource_t>(id, filepath);
		}

		template<typename Resource_t>
		[[nodiscard]] Resource<Resource_t> GetOrEmplace(const fs::path& filepath)
		{
			Handle id = RESNAME(filepath.filename().string());
			return Exists(id) ? Get<Resource_t>(id) : Insert<Resource_t>(id, filepath);
		}

		template<typename Resource_t>
		auto Iterate()
		{
			auto& resourcePool = GetPoolToUse<Resource_t>();
			return Iterable(resourcePool);
		}

		bool Exists(Handle id);
		void Load(Handle id);
		void LoadAllResources();
		template<typename Resource_t>
		void LoadAll() { static_assert("Unknown type!"); }

		void Clear();

		void CreateSnapshot(ResourceManager& snapshotTarget) const;
		void LoadSnapshot(const ResourceManager& snapshot);


		// Dependency Graph
		void AddDependency(Handle dependent, Handle dependency);
		Iterable<std::vector<Handle>> SolveDependencies();

	protected:
		void TopologicalSort();

	private:
		template<typename T>
		Pool<T>& GetPoolToUse() { static_assert("Unknown type!"); }

	private:
		std::unordered_map<Handle, IndexData> m_Registry;
		std::unordered_map<Handle, std::list<Handle>> m_DependencyGraph;

		// Resource Pools
		Pool<Texture> m_TexturePool;
		Pool<Shader> m_ShaderPool;
		Pool<AScriptFactory> m_ScriptFactoryPool;
		Pool<Mesh> m_MeshPool;
		Pool<Material> m_MaterialPool;

		
		std::vector<Handle> m_SortedOrder;
		bool m_Unsorted = true; // similar to dirty bit

		friend class ResourceSerializer;
	};

	template<>
	inline ResourceManager::Pool<Texture>& ResourceManager::GetPoolToUse<Texture>()
	{
		return m_TexturePool;
	}

	template<>
	inline ResourceManager::Pool<Shader>& ResourceManager::GetPoolToUse<Shader>()
	{
		return m_ShaderPool;
	}

	template<>
	inline ResourceManager::Pool<AScriptFactory>& ResourceManager::GetPoolToUse<AScriptFactory>()
	{
		return m_ScriptFactoryPool;
	}

	template<>
	inline ResourceManager::Pool<Mesh>& ResourceManager::GetPoolToUse<Mesh>()
	{
		return m_MeshPool;
	}

	template<>
	inline ResourceManager::Pool<Material>& ResourceManager::GetPoolToUse<Material>()
	{
		return m_MaterialPool;
	}
	

	template<>
	void ResourceManager::LoadAll<Shader>();

	template<>
	void ResourceManager::LoadAll<Texture>();

}
