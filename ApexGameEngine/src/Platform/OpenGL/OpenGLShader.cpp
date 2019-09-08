#include "apex_pch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Apex {

	OpenGLShader::OpenGLShader(const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		const GLchar* source;
		GLint success;

		//Create Vertex Shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		source = (const GLchar*)vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, nullptr);
		glCompileShader(vertexShader);

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			GLchar* infoLog = new GLchar[maxLength];
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, infoLog);
			glDeleteShader(vertexShader);

			APEX_CORE_ERROR("{0}", infoLog);
			APEX_CORE_ASSERT(false, "Vertex shader compilation failed");

			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		source = (const GLchar*)fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, nullptr);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			GLchar* infoLog = new GLchar[maxLength];
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, infoLog);

			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			APEX_CORE_ERROR("{0}", infoLog);
			APEX_CORE_ASSERT(false, "Fragment shader compilation failed");

			return;
		}


		m_RendererID = glCreateProgram();
		GLuint program = m_RendererID;

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (success == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			GLchar* infoLog = new GLchar[maxLength];
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

			glDeleteProgram(program);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			APEX_CORE_ERROR("{0}", infoLog);
			APEX_CORE_ASSERT(false, "Shader program linking failed");

			return;
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);

		OpenGLShader::GetActiveUniformLocations();

	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}
	
	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	
	void OpenGLShader::Unbind() const
	{
	}

	void OpenGLShader::GetActiveUniformLocations()
	{
		int numUniforms = -1;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &numUniforms);

		for (int i = 0; i < numUniforms; i++) {
			
			int namelen = -1, num = -1;
			GLenum type = GL_ZERO;
			char name[512];

			glGetActiveUniform(m_RendererID,
				static_cast<uint32_t>(i),
				sizeof(name)-1,
				&namelen,
				&num,
				&type,
				name
				);
			name[namelen] = 0;

			uint32_t location = glGetUniformLocation(m_RendererID, name);

			m_UniformLocations.emplace(name, location);
		}
	}
	
#ifndef SHADER_UNIFORMS_NO_CACHE
#define SHADER_UNIFORMS_NO_CACHE
#endif


	void OpenGLShader::SetUniInt(const std::string & name, int value)
	{
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	#else
		glUniform1i(m_UniformLocations.at(name), value);
	#endif
	}

	void OpenGLShader::SetUniFloat1(const std::string & name, float value)
	{
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	#else
		glUniform1f(m_UniformLocations.at(name), value);
	#endif
	}
	void OpenGLShader::SetUniFloat2(const std::string & name, const glm::vec2 & value)
	{
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform2fv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat3(const std::string & name, const glm::vec3 & value)
	{
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform3fv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat4(const std::string & name, const glm::vec4 & value)
	{
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform4fv(m_UniformLocations.at(name), 1, glm::value_ptr(value)); 
	#endif
	}

	void OpenGLShader::SetUniMat4(const std::string& name, const glm::mat4 & matrix)
	{
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	#else
		glUniformMatrix4fv(m_UniformLocations.at(name), 1, GL_FALSE, glm::value_ptr(matrix));
	#endif
	}

#ifdef SHADER_UNIFORMS_NO_CACHE
#undef SHADER_UNIFORMS_NO_CACHE
#endif

}