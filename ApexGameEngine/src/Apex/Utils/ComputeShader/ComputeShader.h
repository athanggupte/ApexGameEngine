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

		virtual void Dispatch(uint32_t nWorkGrpsX, uint32_t nWorkGrpsY, uint32_t nWorkGrpsZ) = 0;

		virtual void SetUniInt(const std::string& name, int value) = 0;
		virtual void SetUniFloat1(const std::string& name, float value) = 0;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) = 0;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) = 0;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) = 0;

		static Ref<ComputeShader> Create(const std::string& filepath);
		static Ref<ComputeShader> Create(const std::string& name, const std::string& source);
	};

}