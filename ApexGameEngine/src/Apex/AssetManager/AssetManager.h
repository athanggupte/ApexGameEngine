#pragma once

#include "Apex/Graphics/RenderPrimitives/Shader.h"

namespace Apex {

	class TextureLibrary {};
	class MaterialLibrary {};
	class MeshLibrary {};

	class AssetManager
	{
	public:
		virtual ~AssetManager();

		static void Init();

		//static 
		static ShaderLibrary& GetShaderLibrary() { return s_Instance->m_ShaderLib; }
		static TextureLibrary& GetTextureLibrary() { return s_Instance->m_TextureLib; }
		static MaterialLibrary& GetMaterialLibrary() { return s_Instance->m_MaterialLib; }
		static MeshLibrary& GetMeshLibrary() { return s_Instance->m_MeshLib; }

	private:
		AssetManager();

	private:
		ShaderLibrary m_ShaderLib;
		TextureLibrary m_TextureLib;
		MaterialLibrary m_MaterialLib;
		MeshLibrary m_MeshLib;
		
		static AssetManager* s_Instance;
	};

}