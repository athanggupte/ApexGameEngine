#include "apex_pch.h"
#include "Material.h"

#include "Apex/Application.h"

namespace Apex {

	void Material::Bind() const
	{
		m_Shader->Bind();
		int slot = -1;
		for (auto& [name, texture] : m_Textures) {
			if (texture.IsNull())
				texture->Bind(++slot);
			m_Shader->SetUniInt1(name, slot);
		}
	}
	
	void Material::Unbind() const
	{
	}

	void Material::SetShader(const Resource<Shader>& shader)
	{
		if (shader.GetId() == m_Shader.GetId())
			return;

		m_Shader = shader;
		m_Textures.clear();
		Application::Get().GetResourceManager().Load(m_Shader.GetId());
		APEX_CORE_INFO("Shader {0} uniforms:", Strings::Get(m_Shader.GetId()));
		for (auto [name, type, location, size] : m_Shader->GetActiveUniformData()) {
			APEX_CORE_INFO("\t{0} | type: {1} | location: {2} | size: {3}", name, ShaderUniformTypeToString(type), location, size);
		
			if (ShaderUniformTypeIsSampler(type))
				m_Textures[name] = Resource<Texture>();
		}
	}

	void Material::SetTexture(const std::string & name, const Resource<Texture>& texture)
	{
		APEX_CORE_TRACE("uniform name : {0}", name);
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		itr->second = texture;
	}
	
	Resource<Texture> Material::GetTexture(const std::string & name) const
	{
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		return itr->second;
	}

	bool Material::TextureExists(const std::string & name) const
	{
		return m_Textures.find(name) != m_Textures.end();
	}
	
}
