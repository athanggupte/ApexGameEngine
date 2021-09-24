#include "apexed_pch.h"
#include <Apex.h>
#include "Apex/StartUp.h"

#include "EditorLayer.h"
#include "Apex/Utils/Utils.h"

namespace Apex {

	class ApexEditor : public Application
	{
	public:
		ApexEditor()
			: Application(WindowProps("ApexEditor", 1366u, 768u))
		{
			PushLayer(new EditorLayer());
		}
	};


	Application* CreateApplication()
	{
		return new ApexEditor();
	}

}
