#include "apex_pch.h"
#include "AssetManager.h"

namespace Apex {

	AssetManager* AssetManager::s_Instance = nullptr;

	AssetManager::AssetManager()
	{
		//m_ShaderLib.reset();
		//m_TextureLib.reset();
		//m_MaterialLib.reset();
		//m_MeshLib.reset();
	}

	AssetManager::~AssetManager()
	{
	}

	void AssetManager::Init()
	{
		s_Instance = new AssetManager();
	}

	void AssetManager::Shutdown()
	{
		delete s_Instance;
	}
	
	// Resource Getters
		
	template<>
	const Ref<Shader>& AssetManager::GetResourceByID<Shader>(const GUID& guid)
	{
		return s_Instance->m_ShaderLib.GetResourceByID(guid);
	}
	
	template<>
	const Ref<Texture>& AssetManager::GetResourceByID<Texture>(const GUID& guid)
	{
		return s_Instance->m_TextureLib.GetResourceByID(guid);
	}
	
	
	// Library Getters
	
	template<>
	ResourceLibrary<Shader>& AssetManager::GetResourceLibrary()
	{
		return s_Instance->m_ShaderLib;
	}
	
	template<>
	ResourceLibrary<Texture>& AssetManager::GetResourceLibrary()
	{
		return s_Instance->m_TextureLib;
	}
	
	
}
