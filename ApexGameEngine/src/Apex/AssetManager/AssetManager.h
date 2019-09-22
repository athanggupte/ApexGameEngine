#pragma once

#include "Apex/Renderer/Shader.h"

namespace Apex {

	class TextureLibrary;
	class MaterialLibrary;
	class MeshLibrary;

	class AssetManager
	{
	public:
		virtual ~AssetManager();

		static void Init();

		//static 
		static ShaderLibrary& GetShaderLibrary() { return s_Instance->m_ShaderLib; }
		static Ref<TextureLibrary> GetTextureLibrary() { return s_Instance->m_TextureLib; }
		static Ref<MaterialLibrary> GetMaterialLibrary() { return s_Instance->m_MaterialLib; }
		static Ref<MeshLibrary> GetMeshLibrary() { return s_Instance->m_MeshLib; }

	private:
		AssetManager();

	private:
		ShaderLibrary m_ShaderLib;
		Ref<TextureLibrary> m_TextureLib;
		Ref<MaterialLibrary> m_MaterialLib;
		Ref<MeshLibrary> m_MeshLib;
		
		static AssetManager* s_Instance;
	};

}