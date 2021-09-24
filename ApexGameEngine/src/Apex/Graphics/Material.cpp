#include "apex_pch.h"
#include "Material.h"

namespace Apex {

	void Material::Bind()
	{
		m_Shader->Bind();
		int slot = -1;
		for (auto& texturePair : m_Textures) {
			texturePair.second->Bind(++slot);
			m_Shader->SetUniInt1("u_" + texturePair.first, slot);
		}
	}
	
	void Material::Unbind()
	{
	}
	
	void Material::AddTexture(const std::string & name, const Ref<Texture>& texture)
	{
		auto& shaderUniforms = m_Shader->GetActiveUniformLocations();
		auto str = "u_" + name;
		APEX_CORE_TRACE("uniform name : {0}", str);
		APEX_CORE_ASSERT(shaderUniforms.find(str) != shaderUniforms.end(), "Texture does not map to any uniform in attached Shader");
		m_Textures[name] = texture;
	}
	
	Ref<Texture> Material::GetTexture(const std::string & name)
	{
		APEX_CORE_ASSERT(TextureExists(name), "Texture does not exist");
		return m_Textures[name];
	}

	bool Material::TextureExists(const std::string & name)
	{
		return m_Textures.find(name) != m_Textures.end();
	}
}
