#include <apex_pch.h>
#include "OpenGLComputeShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Apex {

    OpenGLComputeShader::OpenGLComputeShader(const std::string& filepath)
    {
		std::string source;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			source.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&source[0], source.size());
			in.close();
		}
		else {
			APEX_CORE_ERROR("Could not open file : {0}", filepath);
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);

		Compile(source);
    }

	OpenGLComputeShader::OpenGLComputeShader(const std::string& name, const std::string& source)
		: m_Name(name)
	{
		Compile(source);
	}

	OpenGLComputeShader::~OpenGLComputeShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLComputeShader::Bind()
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLComputeShader::Unbind()
	{
	}

	void OpenGLComputeShader::Dispatch(uint32_t nWorkGrpsX, uint32_t nWorkGrpsY, uint32_t nWorkGrpsZ)
	{
		glDispatchCompute(nWorkGrpsX, nWorkGrpsY, nWorkGrpsZ);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void OpenGLComputeShader::Compile(const std::string& source)
	{
		GLuint program = glCreateProgram();
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		const GLchar* src = (const GLchar*)source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		{
			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (success == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				GLchar* infoLog = new GLchar[maxLength];
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);
				
				glDeleteShader(shader);
				glDeleteProgram(program);

				APEX_CORE_ERROR("Compute Shader \"{0}\" : {1}", m_Name, infoLog);
				APEX_CORE_CRITICAL("Compute shader compilation failed");

				return;
			}
		}

		glAttachShader(program, shader);
		glLinkProgram(program);

		{
			GLint success;
			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if (success == GL_FALSE) {
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				GLchar* infoLog = new GLchar[maxLength];
				glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

				glDeleteProgram(program);
				glDeleteShader(shader);

				APEX_CORE_ERROR("Compute Shader \"{0}\" : {1}", m_Name, infoLog);
				APEX_CORE_CRITICAL("Compute shader program linking failed");

				return;
			}
		}

		m_RendererID = program;

		glDetachShader(program, shader);
		glDeleteShader(shader);
	}

}