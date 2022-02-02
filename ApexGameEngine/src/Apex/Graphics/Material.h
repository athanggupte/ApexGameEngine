#pragma once

#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/CustomDataStructures/Iterable.hpp"

namespace Apex {

	class APEX_API Material
	{
	public:
		Material() = default;
		Material(const fs::path&) {}
		virtual ~Material() = default;

		virtual void Bind() const;
		virtual void Unbind() const;

		void SetShader(const Resource<Shader>& shader);
		[[nodiscard]] Resource<Shader> GetShader() const { return m_Shader; }

		void SetTexture(const std::string& name, const Resource<Texture>& texture);
		[[nodiscard]] Resource<Texture> GetTexture(const std::string& name) const;
		[[nodiscard]] bool TextureExists(const std::string& name) const;

		auto GetTextures() { return Iterable(m_Textures); }

	private:
		Resource<Shader> m_Shader;
		std::unordered_map<std::string, Resource<Texture>> m_Textures;
	};

}
