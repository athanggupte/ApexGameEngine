#include "apex_pch.h"
#include "AssetManager.h"

namespace Apex {

	AssetManager* AssetManager::s_Instance = nullptr;

	AssetManager::AssetManager()
	{
		//m_ShaderLib.reset();
		m_TextureLib.reset();
		m_MaterialLib.reset();
		m_MeshLib.reset();
	}

	AssetManager::~AssetManager()
	{
	}

	void AssetManager::Init()
	{
		s_Instance = new AssetManager();
	}

}