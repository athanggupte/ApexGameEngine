#pragma once

#include "Apex/Renderer/Shader.h"
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
		virtual void GetActiveUniformLocations();

		/* Uniform Setters */
		virtual void SetUniInt(const std::string & name, int value) override;
		virtual void SetUniFloat1(const std::string & name, float value) override;
		virtual void SetUniFloat2(const std::string & name, const glm::vec2 & value) override;
		virtual void SetUniFloat3(const std::string & name, const glm::vec3 & value) override;
		virtual void SetUniFloat4(const std::string & name, const glm::vec4 & value) override;
		virtual void SetUniMat4(const std::string & name, const glm::mat4 & matrix) override;

	private:
		std::unordered_map<GLenum, std::string> ParseSource(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
		std::map<std::string, uint32_t> m_UniformLocations;
	};

}