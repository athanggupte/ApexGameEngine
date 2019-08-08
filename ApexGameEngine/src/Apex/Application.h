#pragma once

#include "Core.h"

namespace Apex {

	class APEX_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};


	//To be defined in client
	Application* CreateApplication();

}
