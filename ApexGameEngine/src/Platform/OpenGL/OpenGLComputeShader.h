#pragma once

#include "Apex/ComputeShader/ComputeShader.h"

namespace Apex {

	class OpenGLComputeShader : public ComputeShader
	{
	public:


	private:

		// Inherited via ComputeShader
		virtual void MakeCurrent() override;
	};

}