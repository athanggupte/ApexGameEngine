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
	
	void OpenGLShader::SetUniFloat1(const std::string & name, float value)
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::SetUniMat4(const std::string& name, const glm::mat4 & matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}
}