#pragma once

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include <glad/glad.h>

namespace fs = std::filesystem;

namespace Apex {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const fs::path& filepath);
		OpenGLShader(std::string name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader() override;

		void Bind() const override;
		void Unbind() const override;

		[[nodiscard]] const std::string& GetName() const override { return m_Name; }
		bool IsBound() const override;
		bool IsValid() const override { return m_RendererID; };

		/* Get locations of all active Uniforms */
		void UpdateActiveUniformLocations();
		[[nodiscard]] const std::unordered_map<std::string, uint32_t>& GetActiveUniformLocations() const override;
		[[nodiscard]] const std::vector<ShaderReflection::UniformData> GetActiveUniformData() const override;

		/* Uniform Setters */
		// Int types
		void SetUniInt1(const std::string& name, int value) const override;
		void SetUniInt2(const std::string& name, const glm::ivec2& value) const override;
		void SetUniInt3(const std::string& name, const glm::ivec3& value) const override;
		void SetUniInt4(const std::string& name, const glm::ivec4& value) const override;
		
		// Int array types
		void SetUniInt1v(const std::string& name, int values[], size_t count) const override;
		void SetUniInt2v(const std::string& name, glm::ivec2 values[], size_t count) const override;
		void SetUniInt3v(const std::string& name, glm::ivec3 values[], size_t count) const override;
		void SetUniInt4v(const std::string& name, glm::ivec4 values[], size_t count) const override;
		
		// Float types
		void SetUniFloat1(const std::string& name, float value) const override;
		void SetUniFloat2(const std::string& name, const glm::vec2& value) const override;
		void SetUniFloat3(const std::string& name, const glm::vec3& value) const override;
		void SetUniFloat4(const std::string& name, const glm::vec4& value) const override;
		
		// Float array types
		void SetUniFloat1v(const std::string& name, float values[], size_t count) const override;
		void SetUniFloat2v(const std::string& name, glm::vec2 values[], size_t count) const override;
		void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) const override;
		void SetUniFloat4v(const std::string& name, glm::vec4 values[], size_t count) const override;
		
		// Matrix types
		void SetUniMat4(const std::string& name, const glm::mat4& matrix) const override;
		void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) const override;

	private:
		static void SolveIncludes(std::string& source, const fs::path& filepath, bool hasFilepath = true);
		static std::unordered_map<GLenum, std::string> ParseSource(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		// TODO : remove soon
		GLint my_glGetUniformLocation(GLuint program, const GLchar* name) const;

	private:
		uint32_t m_RendererID = 0;
		std::string m_Name;
		std::unordered_map<std::string, uint32_t> m_UniformLocations;
		
		inline static uint32_t s_BoundShader = -1;
	};

}
