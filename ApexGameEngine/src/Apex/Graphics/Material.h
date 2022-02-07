#pragma once

#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/CustomDataStructures/Iterable.hpp"

namespace Apex {

	class APEX_API Material
	{
	public:
		struct TextureSlotMap
		{
			Resource<Texture> texture;
			uint32_t slot = 0;
			bool use = false;
			glm::vec4 altColor = { 1.f, 1.f, 1.f, 1.f };
			ShaderUniformType altType{ ShaderUniformType::FLOAT };
			bool restrictValues = true;
		};

	public:
		Material();
		explicit Material(const Resource<Shader>& shader);
		explicit Material(const fs::path&);
		virtual ~Material() = default;

		virtual void Bind() const;
		virtual void Unbind() const;

		void SetShader(const Resource<Shader>& shader);
		[[nodiscard]] Resource<Shader> GetShader() const { return m_Shader; }

		void SetTexture(const std::string& name, const Resource<Texture>& texture);
		void SetTextureEnabled(const std::string& name, bool enabled = true);
		void SetAltColor(const std::string& name, const glm::vec4& altColor);
		

		[[nodiscard]] TextureSlotMap GetTextureSlotMap(const std::string& name) const;
		[[nodiscard]] Resource<Texture> GetTexture(const std::string& name) const;
		[[nodiscard]] bool TextureExists(const std::string& name) const;

		auto GetTextures() { return Iterable(m_Textures); }

	private:
		Resource<Shader> m_Shader;
		std::unordered_map<std::string, TextureSlotMap> m_Textures;
	};

}
