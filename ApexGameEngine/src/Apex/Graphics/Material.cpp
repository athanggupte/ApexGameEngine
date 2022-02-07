#include "apex_pch.h"
#include "Material.h"

#include "Apex/Application.h"
#include "ShaderDefines.h"

namespace Apex {

	static Ref<Texture2D> emptyTexture;
	static Ref<Shader> errorShader;

	static uint32_t GetTextureSlotFromName(const std::string& name)
	{
		if (name == "_MatAlbedo")		return TEX_SLOT_MAT_Albedo;
		if (name == "_MatMetallic")		return TEX_SLOT_MAT_Metallic;
		if (name == "_MatRoughness")	return TEX_SLOT_MAT_Roughness;
		if (name == "_MatNormal")		return TEX_SLOT_MAT_Normal;
		return 0;
	}

	Material::Material()
	{
		if (!emptyTexture) {
			constexpr auto errorTextureSpec = TextureSpec{ TextureAccessFormat::RGBA, TextureInternalFormat::RGBA8, TextureDataType::UBYTE };
			emptyTexture = Texture2D::Create(1U, 1U, errorTextureSpec, "ERROR");
			uint32_t errorTextureData = 0xffaaaaaa;
			emptyTexture->SetData(&errorTextureData, sizeof(errorTextureData));
		}
	}

	Material::Material(const Resource<Shader>& shader)
		: Material()
	{
		SetShader(shader);
	}

	Material::Material(const fs::path&)
		: Material()
	{
	}

	void Material::Bind() const
	{
		/*m_Shader->Bind();
		int slot = -1;
		for (auto& [name, texture] : m_Textures) {
			if (texture.IsNull())
				texture->Bind(++slot);
			m_Shader->SetUniInt1(name, slot);
		}*/
		for (auto& [name, map] : m_Textures) {
			if (map.slot < TEX_SLOT_MAT_User0) {
				m_Shader->SetUniInt1("_bUse" + name, map.use);
			}

			if (map.texture.IsValid() && map.use) {
				map.texture->Bind(map.slot);
			} else {
				emptyTexture->Bind(map.slot);
				std::string altColorName = std::string("_alt") + (map.slot < TEX_SLOT_MAT_User0 ? "Mat" : "") + name;
				switch (map.altType)
				{
				case ShaderUniformType::FLOAT:
					m_Shader->SetUniFloat1(altColorName, map.altColor.r);
					break;
				case ShaderUniformType::FLOAT_VEC2:
					m_Shader->SetUniFloat2(altColorName, map.altColor);
					break;
				case ShaderUniformType::FLOAT_VEC3:
					m_Shader->SetUniFloat3(altColorName, map.altColor);
					break;
				case ShaderUniformType::FLOAT_VEC4:
					m_Shader->SetUniFloat4(altColorName, map.altColor);
					break;
				default: 
					break;
				}
			}
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

		uint32_t userTextureSlots = 0;
		for (const auto [name, type, location, size] : m_Shader->GetActiveUniformData()) {
			APEX_CORE_INFO("\t{0} | type: {1} | location: {2} | size: {3}", name, ShaderUniformTypeToString(type), location, size);

			if (ShaderUniformTypeIsSampler(type)) {
				TextureSlotMap map;
				std::string mapId = name;
				if (name[0] == '_') {
					// Uniform is a reserved texture
					auto texType = name.substr(0, 4);
					if (texType == "_Mat") {
						// Material texture
						mapId = name.substr(4);
						map.slot = GetTextureSlotFromName(name);
					} else if (texType == "_Env") {
						// Environment texture
						continue; // Don't add this type of texture to Material
					}
				} else {
					// Uniform is a user-defined texture
					map.slot = TEX_SLOT_MAT_User0 + userTextureSlots;
				}
				// If mapping with texture name exists then assign the texture to the map,
				// else create a new mapping for the texture name
				if (auto itr = m_Textures.find(mapId); itr != m_Textures.end()) {
					itr->second.slot = map.slot;
				} else {
					auto [it, success] = m_Textures.try_emplace(mapId, map);
					APEX_CORE_ASSERT(success, "Texture slot could not be added to material!");
				}
				APEX_CORE_DEBUG("mapId: {}", mapId);
			} else if (name.find("_alt", 0, 4) == 0) {
				// Uniform is an alternative for a texture
				TextureSlotMap map;
				map.altType = type;
				std::string mapId = name.substr(4);
				auto texType = name.substr(4, 3);
				if (texType == "Mat") {
					// Material texture
					mapId = name.substr(7);
				} else if (texType == "Env") {
					// Environment texture
					continue; // Don't add this type of texture to Material
				}
				// If mapping with texture name exists then assign the texture to the map,
				// else create a new mapping for the texture name
				if (auto itr = m_Textures.find(mapId); itr != m_Textures.end()) {
					itr->second.altType = map.altType;
				} else {
					auto [it, success] = m_Textures.try_emplace(mapId, map);
					APEX_CORE_ASSERT(success, "Texture slot could not be added to material!");
				}
			}
		}

		std::stringstream ss;
		ss << fmt::format("[Material] :: shader : {}", m_Shader->GetName()) << '\n';
		ss << "\tTextures : " << '\n';
		for (auto& [name, map] : m_Textures) {
			ss << name << '\n';
		}
		APEX_CORE_WARN("{}", ss.str());
	}

	void Material::SetTexture(const std::string & name, const Resource<Texture>& texture)
	{
		APEX_CORE_TRACE("uniform name : {0}", name);
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		itr->second.texture = texture;
		itr->second.use = true;
	}

	void Material::SetTextureEnabled(const std::string& name, bool enabled)
	{
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		itr->second.use = enabled;
	}

	void Material::SetAltColor(const std::string& name, const glm::vec4& altColor)
	{
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		itr->second.altColor = altColor;
	}

	Material::TextureSlotMap Material::GetTextureSlotMap(const std::string& name) const
	{
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		return itr->second;
	}

	Resource<Texture> Material::GetTexture(const std::string & name) const
	{
		const auto itr = m_Textures.find(name);
		APEX_CORE_ASSERT(itr != m_Textures.end(), "Texture does not exist in Shader");
		return itr->second.texture;
	}

	bool Material::TextureExists(const std::string & name) const
	{
		return m_Textures.find(name) != m_Textures.end();
	}
	
}
