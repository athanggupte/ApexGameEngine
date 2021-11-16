#include "apex_pch.h"
#include "OpenGLShader.h"

#include "Apex/Utils/Utils.h"
#include "Apex/Core/FileSystem/FileSystem.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

// #include <shaderc/shaderc.hpp>
#include <utility>

namespace Apex {

	static GLenum ShaderStageFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "geometry")
			return GL_GEOMETRY_SHADER;
		if (type == "tess_control")
			return GL_TESS_CONTROL_SHADER;
		if (type == "tess_eval")
			return GL_TESS_EVALUATION_SHADER;

		APEX_CORE_CRITICAL("Unknown shader type '{0}' in shader!", type);
		return 0;
	}

	static std::string ShaderStageToString(GLenum stage)
	{
		switch (stage) {
		case GL_VERTEX_SHADER: return "VERTEX";
		case GL_FRAGMENT_SHADER: return "FRAGMENT";
		case GL_GEOMETRY_SHADER: return "GEOMETRY";
		case GL_TESS_CONTROL_SHADER: return "TESSELLATION_CONTROL";
		case GL_TESS_EVALUATION_SHADER: return "TESSELLATION_EVALUATION";
		}

		APEX_CORE_CRITICAL("Unknown shader stage enum ({0})!", stage);
	}

	static ShaderUniformType GetOpenGLShaderUniformType(GLenum dataType);

	OpenGLShader::OpenGLShader(const fs::path& filepath)
		: m_Name(filepath.string())
	{
		std::string source;

		if (const auto file = FileSystem::GetFileIfExists(filepath); file && file->OpenRead()) {
			source.resize(file->Size());
			file->Read(&source[0], source.size());
			file->Close();
		}

		auto shaderSources = ParseSource(source);
		Compile(shaderSources);
		
		glObjectLabel(GL_PROGRAM, m_RendererID, -1, m_Name.c_str());
	}

	OpenGLShader::OpenGLShader(std::string name, const std::string & vertexSrc, const std::string & fragmentSrc)
		: m_Name(std::move(name))
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
		const size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			APEX_CORE_ASSERT(eol != std::string::npos, "Syntax Error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			APEX_CORE_ASSERT(ShaderStageFromString(type), "Invalid shader type specified : " + type);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderStageFromString(type)] =
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

				APEX_CORE_ERROR("Shader \"{0}\" ({2}) : {1}", m_Name, infoLog, ShaderStageToString(type));
				APEX_CORE_CRITICAL("Shader compilation failed!");

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
			APEX_CORE_CRITICAL("Shader program linking failed!");

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
				sizeof name-1,
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

	const std::vector<ShaderReflection::UniformData> OpenGLShader::GetActiveUniformData() const
	{
		std::vector<ShaderReflection::UniformData> uniformData;
		int numUniforms = -1;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &numUniforms);

		for (int i = 0; i < numUniforms; i++) {

			int namelen = -1, num = -1;
			GLenum type = GL_ZERO;
			char name[512];

			glGetActiveUniform(m_RendererID,
				static_cast<uint32_t>(i),
				sizeof name - 1,
				&namelen,
				&num,
				&type,
				name
			);
			name[namelen] = 0;

			int32_t location = glGetUniformLocation(m_RendererID, name);

			ShaderReflection::UniformData data = { std::string(name), GetOpenGLShaderUniformType(type), location, num };
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
	#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform1i(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), value);
	#else
		glProgramUniform1i(m_RendererID, m_UniformLocations.at(name), value);
	#endif
	}

	void OpenGLShader::SetUniInt2(const std::string & name, const glm::ivec2& value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform2iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glProgramUniform2iv(m_RendererID, m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniInt3(const std::string & name, const glm::ivec3& value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform3iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glProgramUniform3iv(m_RendererID, m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniInt4(const std::string & name, const glm::ivec4& value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform4iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glProgramUniform4iv(m_RendererID, m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	//////////     Integer Array Types     /////////////
	void OpenGLShader::SetUniInt1v(const std::string & name, int values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform1iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	#else
		glProgramUniform1iv(m_RendererID, m_UniformLocations.at(name), count, values);
	#endif
	}

	void OpenGLShader::SetUniInt2v(const std::string & name, glm::ivec2 values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform2iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glProgramUniform2iv(m_RendererID, m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}

	void OpenGLShader::SetUniInt3v(const std::string & name, glm::ivec3 values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform3iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glProgramUniform3iv(m_RendererID, m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}

	void OpenGLShader::SetUniInt4v(const std::string & name, glm::ivec4 values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform4iv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glProgramUniform4iv(m_RendererID, m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}
	
	//////////////////////////////////////////////////////////////
	//////////////////      Float Types      /////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLShader::SetUniFloat1(const std::string & name, float value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform1f(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), value);
	#else
		glProgramUniform1f(m_RendererID, m_UniformLocations.at(name), value);
	#endif
	}

	void OpenGLShader::SetUniFloat2(const std::string & name, const glm::vec2 & value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform2fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glProgramUniform2fv(m_RendererID, m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat3(const std::string & name, const glm::vec3 & value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform3fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glProgramUniform3fv(m_RendererID, m_UniformLocations.at(name), 1, glm::value_ptr(value));
	#endif
	}

	void OpenGLShader::SetUniFloat4(const std::string & name, const glm::vec4 & value) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform4fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
	#else
		glProgramUniform4fv(m_RendererID, m_UniformLocations.at(name), 1, glm::value_ptr(value)); 
	#endif
	}

	//////////     Float Array Types     /////////////
	void OpenGLShader::SetUniFloat1v(const std::string & name, float values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform1fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, values);
	#else
		glProgramUniform1fv(m_RendererID, m_UniformLocations.at(name), count, values);
	#endif
	}
	
	void OpenGLShader::SetUniFloat2v(const std::string & name, glm::vec2 values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform2fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glProgramUniform2fv(m_RendererID, m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}

	void OpenGLShader::SetUniFloat3v(const std::string & name, glm::vec3 values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform3fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glProgramUniform3fv(m_RendererID, m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}

	void OpenGLShader::SetUniFloat4v(const std::string & name, glm::vec4 values[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniform4fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, glm::value_ptr(values[0]));
	#else
		glProgramUniform4fv(m_RendererID, m_UniformLocations.at(name), count, glm::value_ptr(values[0]));
	#endif
	}
	
	//////////////////////////////////////////////////////////////
	/////////////////      Matrix Types      /////////////////////
	//////////////////////////////////////////////////////////////
	void OpenGLShader::SetUniMat4(const std::string& name, const glm::mat4 & matrix) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniformMatrix4fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	#else
		glProgramUniformMatrix4fv(m_RendererID, m_UniformLocations.at(name), 1, GL_FALSE, glm::value_ptr(matrix));
	#endif
	}

	void OpenGLShader::SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) const
	{
#ifdef SHADER_UNIFORMS_NO_CACHE
		glProgramUniformMatrix4fv(m_RendererID, glGetUniformLocation(m_RendererID, name.c_str()), count, GL_FALSE, glm::value_ptr(matrices[0]));
	#else
		glProgramUniformMatrix4fv(m_RendererID, m_UniformLocations.at(name), count, GL_FALSE, glm::value_ptr(matrices[0]));
	#endif
	}

#ifdef SHADER_UNIFORMS_NO_CACHE
#undef SHADER_UNIFORMS_NO_CACHE
#endif


	ShaderUniformType GetOpenGLShaderUniformType(GLenum dataType)
	{
		switch (dataType) {
		case GL_FLOAT: return ShaderUniformType::FLOAT;
		case GL_FLOAT_VEC2: return ShaderUniformType::FLOAT_VEC2;
		case GL_FLOAT_VEC3: return ShaderUniformType::FLOAT_VEC3;
		case GL_FLOAT_VEC4: return ShaderUniformType::FLOAT_VEC4;
		case GL_DOUBLE: return ShaderUniformType::DOUBLE;
		case GL_DOUBLE_VEC2: return ShaderUniformType::DOUBLE_VEC2;
		case GL_DOUBLE_VEC3: return ShaderUniformType::DOUBLE_VEC3;
		case GL_DOUBLE_VEC4: return ShaderUniformType::DOUBLE_VEC4;
		case GL_INT: return ShaderUniformType::INT;
		case GL_INT_VEC2: return ShaderUniformType::INT_VEC2;
		case GL_INT_VEC3: return ShaderUniformType::INT_VEC3;
		case GL_INT_VEC4: return ShaderUniformType::INT_VEC4;
		case GL_UNSIGNED_INT: return ShaderUniformType::UNSIGNED_INT;
		case GL_UNSIGNED_INT_VEC2: return ShaderUniformType::UNSIGNED_INT_VEC2;
		case GL_UNSIGNED_INT_VEC3: return ShaderUniformType::UNSIGNED_INT_VEC3;
		case GL_UNSIGNED_INT_VEC4: return ShaderUniformType::UNSIGNED_INT_VEC4;
		case GL_BOOL: return ShaderUniformType::BOOL;
		case GL_BOOL_VEC2: return ShaderUniformType::BOOL_VEC2;
		case GL_BOOL_VEC3: return ShaderUniformType::BOOL_VEC3;
		case GL_BOOL_VEC4: return ShaderUniformType::BOOL_VEC4;
		case GL_FLOAT_MAT2: return ShaderUniformType::FLOAT_MAT2;
		case GL_FLOAT_MAT3: return ShaderUniformType::FLOAT_MAT3;
		case GL_FLOAT_MAT4: return ShaderUniformType::FLOAT_MAT4;
		case GL_FLOAT_MAT2x3: return ShaderUniformType::FLOAT_MAT2x3;
		case GL_FLOAT_MAT2x4: return ShaderUniformType::FLOAT_MAT2x4;
		case GL_FLOAT_MAT3x2: return ShaderUniformType::FLOAT_MAT3x2;
		case GL_FLOAT_MAT3x4: return ShaderUniformType::FLOAT_MAT3x4;
		case GL_FLOAT_MAT4x2: return ShaderUniformType::FLOAT_MAT4x2;
		case GL_FLOAT_MAT4x3: return ShaderUniformType::FLOAT_MAT4x3;
		case GL_SAMPLER_1D: return ShaderUniformType::SAMPLER_1D;
		case GL_SAMPLER_2D: return ShaderUniformType::SAMPLER_2D;
		case GL_SAMPLER_3D: return ShaderUniformType::SAMPLER_3D;
		case GL_SAMPLER_CUBE: return ShaderUniformType::SAMPLER_CUBE;
		case GL_SAMPLER_1D_SHADOW: return ShaderUniformType::SAMPLER_1D_SHADOW;
		case GL_SAMPLER_2D_SHADOW: return ShaderUniformType::SAMPLER_2D_SHADOW;
		case GL_SAMPLER_1D_ARRAY: return ShaderUniformType::SAMPLER_1D_ARRAY;
		case GL_SAMPLER_2D_ARRAY: return ShaderUniformType::SAMPLER_2D_ARRAY;
		case GL_SAMPLER_1D_ARRAY_SHADOW: return ShaderUniformType::SAMPLER_1D_ARRAY_SHADOW;
		case GL_SAMPLER_2D_ARRAY_SHADOW: return ShaderUniformType::SAMPLER_2D_ARRAY_SHADOW;
		case GL_SAMPLER_2D_MULTISAMPLE: return ShaderUniformType::SAMPLER_2D_MULTISAMPLE;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return ShaderUniformType::SAMPLER_2D_MULTISAMPLE_ARRAY;
		case GL_SAMPLER_CUBE_SHADOW: return ShaderUniformType::SAMPLER_CUBE_SHADOW;
		case GL_SAMPLER_BUFFER: return ShaderUniformType::SAMPLER_BUFFER;
		case GL_SAMPLER_2D_RECT: return ShaderUniformType::SAMPLER_2D_RECT;
		case GL_SAMPLER_2D_RECT_SHADOW: return ShaderUniformType::SAMPLER_2D_RECT_SHADOW;
		default: APEX_CORE_CRITICAL("Unknown Shader uniform type!");
		}
	}


}
