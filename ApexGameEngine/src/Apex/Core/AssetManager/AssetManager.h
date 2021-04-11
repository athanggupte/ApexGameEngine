#pragma once

//#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Core/GUID.h"
#include "ResourceLibrary.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {
	
	class AssetManager
	{
	public:
		virtual ~AssetManager();

		static void Init();
		static void Shutdown();
		
		template<typename Resource_t>
		static const Ref<Resource_t>& GetResourceByID(const GUID& guid);

		template<typename Resource_t>
		static ResourceLibrary<Resource_t>& GetResourceLibrary();
		
	private:
		AssetManager();

	private:
		ResourceLibrary<Shader> m_ShaderLib;
		ResourceLibrary<Texture> m_TextureLib;
// 		ResourceLibrary<Material> m_MaterialLib;
// 		ResourceLibrary<Mesh> m_MeshLib;
		
		static AssetManager* s_Instance;
	};

}

#include "AssetManager.hpp"
