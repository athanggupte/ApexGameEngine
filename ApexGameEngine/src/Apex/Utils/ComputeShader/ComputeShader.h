#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Apex {

	class ComputeShader
	{
	public:

		virtual ~ComputeShader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const std::string& GetName() const = 0;

		virtual void Dispatch(uint32_t nWorkGrpsX, uint32_t nWorkGrpsY, uint32_t nWorkGrpsZ) = 0;

		// Int types
		virtual void SetUniInt1(const std::string& name, int value) = 0;
		virtual void SetUniInt2(const std::string& name, const glm::ivec2& value) = 0;
		virtual void SetUniInt3(const std::string& name, const glm::ivec3& value) = 0;
		virtual void SetUniInt4(const std::string& name, const glm::ivec4& value) = 0;
		
		// Int array types
		virtual void SetUniInt1v(const std::string& name, int values[], size_t count) = 0;
		virtual void SetUniInt2v(const std::string& name, glm::ivec2 values[], size_t count) = 0;
		virtual void SetUniInt3v(const std::string& name, glm::ivec3 values[], size_t count) = 0;
		virtual void SetUniInt4v(const std::string& name, glm::ivec4 values[], size_t count) = 0;
		
		// Float types
		virtual void SetUniFloat1(const std::string& name, float value) = 0;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) = 0;
		
		// Float array types
		virtual void SetUniFloat1v(const std::string& name, float values[], size_t count) = 0;
		virtual void SetUniFloat2v(const std::string& name, glm::vec2 values[], size_t count) = 0;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) = 0;
		virtual void SetUniFloat4v(const std::string& name, glm::vec4 values[], size_t count) = 0;
		
		// Matrix types
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) = 0;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) = 0;

		static Ref<ComputeShader> Create(const std::string& filepath);
		static Ref<ComputeShader> Create(const std::string& name, const std::string& source);
	};

}
