#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Apex {

	enum class ShaderUniformType;

	namespace ShaderReflection
	{
		struct UniformData
		{
			std::string Name;
			ShaderUniformType Type;
			int32_t Location;
			int32_t ArraySize;
		};
	}

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual bool IsBound() const = 0;
		virtual bool IsValid() const = 0;

		virtual const std::unordered_map<std::string, uint32_t>& GetActiveUniformLocations() const = 0;
		virtual const std::vector<ShaderReflection::UniformData> GetActiveUniformData() const = 0;

		// Int types
		virtual void SetUniInt1(const std::string& name, int value) const = 0;
		virtual void SetUniInt2(const std::string& name, const glm::ivec2& value) const = 0;
		virtual void SetUniInt3(const std::string& name, const glm::ivec3& value) const = 0;
		virtual void SetUniInt4(const std::string& name, const glm::ivec4& value) const = 0;
		
		// Int array types
		virtual void SetUniInt1v(const std::string& name, int values[], size_t count) const = 0;
		virtual void SetUniInt2v(const std::string& name, glm::ivec2 values[], size_t count) const = 0;
		virtual void SetUniInt3v(const std::string& name, glm::ivec3 values[], size_t count) const = 0;
		virtual void SetUniInt4v(const std::string& name, glm::ivec4 values[], size_t count) const = 0;
		
		// Float types
		virtual void SetUniFloat1(const std::string& name, float value) const = 0;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) const = 0;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) const = 0;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) const = 0;
		
		// Float array types
		virtual void SetUniFloat1v(const std::string& name, float values[], size_t count) const = 0;
		virtual void SetUniFloat2v(const std::string& name, glm::vec2 values[], size_t count) const = 0;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) const = 0;
		virtual void SetUniFloat4v(const std::string& name, glm::vec4 values[], size_t count) const = 0;
		
		// Matrix types
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) const = 0;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) const = 0;
		
		static Ref<Shader> APEX_API Create(const std::filesystem::path& filepath);
		static Ref<Shader> APEX_API Create(const std::string& name, const std::string& source);
		static Ref<Shader> APEX_API Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	
	enum class ShaderUniformType
	{
		FLOAT,
		FLOAT_VEC2,
		FLOAT_VEC3,
		FLOAT_VEC4,

		DOUBLE,
		DOUBLE_VEC2,
		DOUBLE_VEC3,
		DOUBLE_VEC4,
		
		INT,
		INT_VEC2,
		INT_VEC3,
		INT_VEC4,

		UNSIGNED_INT,
		UNSIGNED_INT_VEC2,
		UNSIGNED_INT_VEC3,
		UNSIGNED_INT_VEC4,

		BOOL,
		BOOL_VEC2,
		BOOL_VEC3,
		BOOL_VEC4,

		FLOAT_MAT2,
		FLOAT_MAT3,
		FLOAT_MAT4,
		FLOAT_MAT2x3,
		FLOAT_MAT2x4,
		FLOAT_MAT3x2,
		FLOAT_MAT3x4,
		FLOAT_MAT4x2,
		FLOAT_MAT4x3,

		SAMPLER_1D,
		SAMPLER_2D,
		SAMPLER_3D,
		SAMPLER_CUBE,
		SAMPLER_1D_SHADOW,
		SAMPLER_2D_SHADOW,
		SAMPLER_1D_ARRAY,
		SAMPLER_2D_ARRAY,
		SAMPLER_1D_ARRAY_SHADOW,
		SAMPLER_2D_ARRAY_SHADOW,
		SAMPLER_2D_MULTISAMPLE,
		SAMPLER_2D_MULTISAMPLE_ARRAY,
		SAMPLER_CUBE_SHADOW,
		SAMPLER_BUFFER,
		SAMPLER_2D_RECT,
		SAMPLER_2D_RECT_SHADOW,

		_COUNT // Number of values in ShaderUniformType enum. May also be used for undefined values
	};

	bool ShaderUniformTypeIsSampler(ShaderUniformType type);
	const char* ShaderUniformTypeToString(ShaderUniformType type);

}
