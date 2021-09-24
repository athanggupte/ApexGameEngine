#pragma once

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"

namespace Apex {

	class Material
	{
	public:
		Material() {}
		virtual ~Material() = default;

		virtual void Bind();
		virtual void Unbind();

		inline void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }
		inline Ref<Shader> GetShader() { return m_Shader; }

		void AddTexture(const std::string& name, const Ref<Texture>& texture);
		Ref<Texture> GetTexture(const std::string& name);
		bool TextureExists(const std::string& name);

	private:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
	};

}