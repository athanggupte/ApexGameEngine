#pragma once

#include "Apex/RenderPrimitives/Shader.h"
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
		virtual const std::unordered_map<std::string, uint32_t>& GetActiveUniformLocations() override;
		virtual const std::vector<std::tuple<std::string, uint32_t, size_t>>& GetActiveUniformData() override;

		/* Uniform Setters */
		virtual void SetUniInt(const std::string & name, int value) override;
		virtual void SetUniFloat1(const std::string & name, float value) override;
		virtual void SetUniFloat2(const std::string & name, const glm::vec2 & value) override;
		virtual void SetUniFloat3(const std::string & name, const glm::vec3 & value) override;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) override;
		virtual void SetUniFloat4(const std::string & name, const glm::vec4 & value) override;
		virtual void SetUniMat4(const std::string & name, const glm::mat4 & matrix) override;
		virtual void OpenGLShader::SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) override;

	private:
		std::unordered_map<GLenum, std::string> ParseSource(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
		std::unordered_map<std::string, uint32_t> m_UniformLocations;
	};

}