#pragma once

#include "Apex/ComputeShader/ComputeShader.h"

namespace Apex {

	class OpenGLComputeShader : public ComputeShader
	{
	public:
		OpenGLComputeShader(const std::string& filepath);
		OpenGLComputeShader(const std::string& name, const std::string& source);
		~OpenGLComputeShader();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Dispatch(uint32_t nWorkGrpsX, uint32_t nWorkGrpsY, uint32_t nWorkGrpsZ) override;

		/* Uniform Setters */
		virtual void SetUniInt(const std::string& name, int value) override;
		virtual void SetUniFloat1(const std::string& name, float value) override;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) override;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) override;

	private:
		void Compile(const std::string& source);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};

}