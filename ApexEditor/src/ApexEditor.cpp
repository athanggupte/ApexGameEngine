#include <Apex.h>
#include "Apex/StartUp.h"

#include "EditorLayer.h"

namespace Apex {

	class ApexEditor : public Application
	{
	public:
		ApexEditor()
		{
			PushLayer(new EditorLayer());
			Application::Get().GetWindow().SetWindowIcon(Utils::LoadImage("assets/Apex-Game-Engine-32.png"));
		}
	};


	Application* CreateApplication()
	{
		return new ApexEditor();
	}

}
