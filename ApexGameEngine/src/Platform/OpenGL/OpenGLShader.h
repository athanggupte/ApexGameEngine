#pragma once

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include <glad/glad.h>

namespace Apex {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		/* Get locations of all active Uniforms */
		void UpdateActiveUniformLocations();
		virtual const std::unordered_map<std::string, uint32_t>& GetActiveUniformLocations() const override;
		virtual const std::vector<std::tuple<std::string, uint32_t, size_t>> GetActiveUniformData() const override;

		/* Uniform Setters */
		// Int types
		virtual void SetUniInt1(const std::string& name, int value) const override;
		virtual void SetUniInt2(const std::string& name, const glm::ivec2& value) const override;
		virtual void SetUniInt3(const std::string& name, const glm::ivec3& value) const override;
		virtual void SetUniInt4(const std::string& name, const glm::ivec4& value) const override;
		
		// Int array types
		virtual void SetUniInt1v(const std::string& name, int values[], size_t count) const override;
		virtual void SetUniInt2v(const std::string& name, glm::ivec2 values[], size_t count) const override;
		virtual void SetUniInt3v(const std::string& name, glm::ivec3 values[], size_t count) const override;
		virtual void SetUniInt4v(const std::string& name, glm::ivec4 values[], size_t count) const override;
		
		// Float types
		virtual void SetUniFloat1(const std::string& name, float value) const override;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) const override;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) const override;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) const override;
		
		// Float array types
		virtual void SetUniFloat1v(const std::string& name, float values[], size_t count) const override;
		virtual void SetUniFloat2v(const std::string& name, glm::vec2 values[], size_t count) const override;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) const override;
		virtual void SetUniFloat4v(const std::string& name, glm::vec4 values[], size_t count) const override;
		
		// Matrix types
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) const override;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) const override;

	private:
		std::unordered_map<GLenum, std::string> ParseSource(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		void BindIfNotBound() const;
		
	private:
		uint32_t m_RendererID;
		std::string m_Name;
		std::unordered_map<std::string, uint32_t> m_UniformLocations;
		
		inline static uint32_t s_BoundShader = -1;
	};

}
