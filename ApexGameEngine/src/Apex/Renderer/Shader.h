#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Apex {

	class APEX_API Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniFloat1(const std::string& name, float value) = 0;
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) = 0;
		
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}