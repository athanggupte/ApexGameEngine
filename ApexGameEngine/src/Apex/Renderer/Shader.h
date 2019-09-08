#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Apex {

	class APEX_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniInt(const std::string& name, int value) = 0;
		virtual void SetUniFloat1(const std::string& name, float value) = 0;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3&  value) = 0;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4&  value) = 0;
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) = 0;
		
		static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}