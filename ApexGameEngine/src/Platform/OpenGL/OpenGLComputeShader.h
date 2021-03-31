#pragma once

#include "Apex/Utils/ComputeShader/ComputeShader.h"

namespace Apex {

	class OpenGLComputeShader : public ComputeShader
	{
	public:
		OpenGLComputeShader(const std::string& filepath);
		OpenGLComputeShader(const std::string& name, const std::string& source);
		~OpenGLComputeShader();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const std::string& GetName() const override { return m_Name; }
		
		virtual void Dispatch(uint32_t nWorkGrpsX, uint32_t nWorkGrpsY, uint32_t nWorkGrpsZ) override;

		/* Uniform Setters */
		// Int types
		virtual void SetUniInt1(const std::string& name, int value) override;
		virtual void SetUniInt2(const std::string& name, const glm::ivec2& value) override;
		virtual void SetUniInt3(const std::string& name, const glm::ivec3& value) override;
		virtual void SetUniInt4(const std::string& name, const glm::ivec4& value) override;
		
		// Int array types
		virtual void SetUniInt1v(const std::string& name, int values[], size_t count) override;
		virtual void SetUniInt2v(const std::string& name, glm::ivec2 values[], size_t count) override;
		virtual void SetUniInt3v(const std::string& name, glm::ivec3 values[], size_t count) override;
		virtual void SetUniInt4v(const std::string& name, glm::ivec4 values[], size_t count) override;
		
		// Float types
		virtual void SetUniFloat1(const std::string& name, float value) override;
		virtual void SetUniFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetUniFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniFloat4(const std::string& name, const glm::vec4& value) override;
		
		// Float array types
		virtual void SetUniFloat1v(const std::string& name, float values[], size_t count) override;
		virtual void SetUniFloat2v(const std::string& name, glm::vec2 values[], size_t count) override;
		virtual void SetUniFloat3v(const std::string& name, glm::vec3 values[], size_t count) override;
		virtual void SetUniFloat4v(const std::string& name, glm::vec4 values[], size_t count) override;
		
		// Matrix types
		virtual void SetUniMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void SetUniMat4v(const std::string& name, glm::mat4 matrices[], size_t count) override;

	private:
		void Compile(const std::string& source);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};

}
