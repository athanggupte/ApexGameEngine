#pragma once

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/CustomDataStructures.h"

namespace Apex {

	class Material
	{
	public:
		Material() {}
		Material(const std::string&) {}
		virtual ~Material() = default;

		virtual void Bind() const;
		virtual void Unbind() const;

		void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }
		[[nodiscard]] Ref<Shader> GetShader() const { return m_Shader; }

		void AddTexture(const std::string& name, const Ref<Texture>& texture);
		Ref<Texture> GetTexture(const std::string& name);
		bool TextureExists(const std::string& name) const;

		Iterable<std::unordered_map<std::string, Ref<Texture>>> GetTextures() { return Iterable(m_Textures); }

	private:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
	};

}
