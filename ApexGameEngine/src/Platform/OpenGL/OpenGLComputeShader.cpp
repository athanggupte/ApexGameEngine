#include <apex_pch.h>
#include "OpenGLComputeShader.h"

#include "Apex/Core/FileSystem/FileSystem.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Apex {

    OpenGLComputeShader::OpenGLComputeShader(const std::string& filepath)
    {
		std::string source;
		
		auto file = FileSystem::GetFileIfExists(filepath);
		if (file && file->OpenRead()) {
			source.resize(file->Size());
			file->Read(&source[0], source.size());
			file->Close();
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

#ifdef _MSC_VER
	#pragma warning(disable: 4267)
#endif

	//////////////////////////////////////////////////////////////
	/////////////////      Integer Types      ////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLComputeShader::SetUniInt1(const std::string & name, int value)
	{
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLComputeShader::SetUniInt2(const std::string & name, const glm::ivec2& value)
	{
		glUniform2iv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	}

	void OpenGLComputeShader::SetUniInt3(const std::string & name, const glm::ivec3& value)
	{
		glUniform3iv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	}

	void OpenGLComputeShader::SetUniInt4(const std::string & name, const glm::ivec4& value)
	{
		glUniform4iv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	}

	//////////     Integer Array Types     /////////////
	void OpenGLComputeShader::SetUniInt1v(const std::string & name, int values[], size_t count)
	{
		glUniform1iv(glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	}

	void OpenGLComputeShader::SetUniInt2v(const std::string & name, glm::ivec2 values[], size_t count)
	{
		glUniform2iv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	}

	void OpenGLComputeShader::SetUniInt3v(const std::string & name, glm::ivec3 values[], size_t count)
	{
		glUniform3iv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	}

	void OpenGLComputeShader::SetUniInt4v(const std::string & name, glm::ivec4 values[], size_t count)
	{
		glUniform4iv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	}
	
	//////////////////////////////////////////////////////////////
	//////////////////      Float Types      /////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLComputeShader::SetUniFloat1(const std::string & name, float value)
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLComputeShader::SetUniFloat2(const std::string & name, const glm::vec2 & value)
	{
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	}

	void OpenGLComputeShader::SetUniFloat3(const std::string & name, const glm::vec3 & value)
	{
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	}

	void OpenGLComputeShader::SetUniFloat4(const std::string & name, const glm::vec4 & value)
	{
		glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	}

	//////////     Float Array Types     /////////////
	void OpenGLComputeShader::SetUniFloat1v(const std::string & name, float values[], size_t count)
	{
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	}
	
	void OpenGLComputeShader::SetUniFloat2v(const std::string & name, glm::vec2 values[], size_t count)
	{
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	}

	void OpenGLComputeShader::SetUniFloat3v(const std::string & name, glm::vec3 values[], size_t count)
	{
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	}

	void OpenGLComputeShader::SetUniFloat4v(const std::string & name, glm::vec4 values[], size_t count)
	{
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	}
	
	//////////////////////////////////////////////////////////////
	/////////////////      Matrix Types      /////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLComputeShader::SetUniMat4(const std::string& name, const glm::mat4 & matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLComputeShader::SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count)
	{
			glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), count, GL_FALSE, glm::value_ptr(matrices[0]));
	}

}
