#pragma once

#include "Apex/Core/Core.h"

namespace Apex {

	class Input
	{
	public:	//static methods of the Input interface
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};

}
