#pragma once

#include "Apex/Core.h"

namespace Apex {

	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}