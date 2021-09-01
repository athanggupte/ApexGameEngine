#include "apex_pch.h"
#include "OpenGLShader.h"

#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/VFS.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Apex {

	static GLenum ShaderTypeFromString(const std::string & type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "geometry")
			return GL_GEOMETRY_SHADER;

		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string & filepath)
	{
		std::string source;
#ifdef APEX_USE_VFS
		auto file = FileSystem::GetFileIfExists(filepath);
		if (file && file->OpenRead()) {
			source.resize(file->Size());
			file->Read(&source[0], source.size());
			file->Close();
		}
#else
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
#endif

		// Extract name from filepath
		/*auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);*/
		m_Name = Utils::GetFilename(filepath);

		auto shaderSources = ParseSource(source);
		Compile(shaderSources);
		
		glObjectLabel(GL_PROGRAM, m_RendererID, -1, m_Name.c_str());
	}

	OpenGLShader::OpenGLShader(const std::string & name, const std::string & vertexSrc, const std::string & fragmentSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		Compile(sources);

		glObjectLabel(GL_PROGRAM, m_RendererID, -1, m_Name.c_str());
		//OpenGLShader::GetActiveUniformLocations();

	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}
	
	std::unordered_map<GLenum, std::string> OpenGLShader::ParseSource(const std::string & source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			APEX_CORE_ASSERT(eol != std::string::npos, "Syntax Error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			APEX_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified : " + type);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] =
				source.substr(nextLinePos,
					pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		APEX_CORE_ASSERT(shaderSources.size() < 4, "Max. 4 shaders supported");
		//std::array<GLuint, 4> glShaderIDs;
		std::vector<GLuint> glShaderIDs(shaderSources.size());
		int shaderIndex = 0;
		for (auto& kv : shaderSources) {
			GLenum type = kv.first;
			const std::string& src = kv.second;

			GLuint shader = glCreateShader(type);
			const GLchar* source = (const GLchar*)src.c_str();
			glShaderSource(shader, 1, &source, nullptr);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (success == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				GLchar* infoLog = new GLchar[maxLength];
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);
				glDeleteShader(shader);

				APEX_CORE_ERROR("Shader \"{0}\" : {1}", m_Name, infoLog);
				APEX_CORE_CRITICAL("Shader compilation failed");

				glDeleteProgram(program);

				return;
			}

			glAttachShader(program, shader);
			glShaderIDs[shaderIndex++] = shader;
		}
		
		glLinkProgram(program);

		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (success == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			GLchar* infoLog = new GLchar[maxLength];
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

			glDeleteProgram(program);
			
			for (auto id : glShaderIDs) {
				glDeleteShader(id);
			}

			APEX_CORE_ERROR("Shader \"{0}\" : {1}", m_Name, infoLog);
			APEX_CORE_ASSERT(false, "Shader program linking failed");

			return;
		}
		
		m_RendererID = program;

		for (auto id : glShaderIDs) {
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	void OpenGLShader::Bind() const
	{
		// if (s_BoundShader != m_RendererID)
			glUseProgram(m_RendererID);
		s_BoundShader = m_RendererID;
	}
	
	void OpenGLShader::Unbind() const
	{
		if (s_BoundShader == m_RendererID)
			s_BoundShader = -1;
	}

	inline void OpenGLShader::BindIfNotBound() const
	{
		if (s_BoundShader != m_RendererID)
			glUseProgram(m_RendererID);
		s_BoundShader = m_RendererID;
	}
	
	void OpenGLShader::UpdateActiveUniformLocations()
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
	
	const std::unordered_map<std::string, uint32_t>& OpenGLShader::GetActiveUniformLocations() const
	{
		return m_UniformLocations;
	}

	const std::vector<std::tuple<std::string, uint32_t, size_t>> OpenGLShader::GetActiveUniformData() const
	{
		std::vector<std::tuple<std::string, uint32_t, size_t>> uniformData;
		int numUniforms = -1;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &numUniforms);

		for (int i = 0; i < numUniforms; i++) {

			int namelen = -1, num = -1;
			GLenum type = GL_ZERO;
			char name[512];

			glGetActiveUniform(m_RendererID,
				static_cast<uint32_t>(i),
				sizeof(name) - 1,
				&namelen,
				&num,
				&type,
				name
			);
			name[namelen] = 0;

			//uint32_t location = glGetUniformLocation(m_RendererID, name);

			std::tuple<std::string, uint32_t, size_t> data = { std::string(name), type, num };
			uniformData.push_back(data);
		}
		
		return uniformData;
	}
	
#ifndef SHADER_UNIFORMS_NO_CACHE
#define SHADER_UNIFORMS_NO_CACHE
#endif

	//////////////////////////////////////////////////////////////
	/////////////////      Integer Types      ////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLShader::SetUniInt1(const std::string & name, int value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	#else
		glUniform1i(m_UniformLocations.at(name), value);
	#endif
	}

	void OpenGLShader::SetUniInt2(const std::string & name, const glm::ivec2& value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform2iv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform2iv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniInt3(const std::string & name, const glm::ivec3& value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform3iv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform3iv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniInt4(const std::string & name, const glm::ivec4& value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform4iv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform4iv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	//////////     Integer Array Types     /////////////
	void OpenGLShader::SetUniInt1v(const std::string & name, int values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform1iv(glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	#else
		glUniform1iv(m_UniformLocations.at(name), count, values);
	#endif
	}

	void OpenGLShader::SetUniInt2v(const std::string & name, glm::ivec2 values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform2iv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glUniform2iv(m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}

	void OpenGLShader::SetUniInt3v(const std::string & name, glm::ivec3 values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform3iv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glUniform3iv(m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}

	void OpenGLShader::SetUniInt4v(const std::string & name, glm::ivec4 values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform4iv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glUniform4iv(m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}
	
	//////////////////////////////////////////////////////////////
	//////////////////      Float Types      /////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLShader::SetUniFloat1(const std::string & name, float value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	#else
		glUniform1f(m_UniformLocations.at(name), value);
	#endif
	}

	void OpenGLShader::SetUniFloat2(const std::string & name, const glm::vec2 & value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform2fv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat3(const std::string & name, const glm::vec3 & value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform3fv(m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat4(const std::string & name, const glm::vec4 & value) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glUniform4fv(m_UniformLocations.at(name), 1, glm::value_ptr(value)); 
	#endif
	}

	//////////     Float Array Types     /////////////
	void OpenGLShader::SetUniFloat1v(const std::string & name, float values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform1fv(glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	#else
		glUniform1fv(m_UniformLocations.at(name), count, value);
	#endif
	}
	
	void OpenGLShader::SetUniFloat2v(const std::string & name, glm::vec2 values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform2fv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glUniform2fv(m_UniformLocations.at(name), count, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat3v(const std::string & name, glm::vec3 values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glUniform3fv(m_UniformLocations.at(name), count, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat4v(const std::string & name, glm::vec4 values[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glUniform4fv(m_UniformLocations.at(name), count, glm::value_ptr(value));
	#endif
	}
	
	//////////////////////////////////////////////////////////////
	/////////////////      Matrix Types      /////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLShader::SetUniMat4(const std::string& name, const glm::mat4 & matrix) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	#else
		glUniformMatrix4fv(m_UniformLocations.at(name), 1, GL_FALSE, glm::value_ptr(matrix));
	#endif
	}

	void OpenGLShader::SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) const
	{
		BindIfNotBound();
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), count, GL_FALSE, glm::value_ptr(matrices[0]));
	#else
		glUniformMatrix4fv(m_UniformLocations.at(name), count, GL_FALSE, glm::value_ptr(matrices[0]));
	#endif
	}

#ifdef SHADER_UNIFORMS_NO_CACHE
#undef SHADER_UNIFORMS_NO_CACHE
#endif
}
