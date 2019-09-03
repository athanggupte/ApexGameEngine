#pragma once

#include "Apex/Renderer/Shader.h"

namespace Apex {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const;
		virtual void Unbind() const;

		/* Uniform Setters */
		virtual void SetUniFloat1(const std::string& name, float value) override;
		virtual void SetUniMat4(const std::string& name, const glm::mat4 & matrix) override;

	private:
		uint32_t m_RendererID;

		// Inherited via Shader
	};

}