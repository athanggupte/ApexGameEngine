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

	private:
		void Compile(const std::string& source);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};

}