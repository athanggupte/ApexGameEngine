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
		{
			PushLayer(new EditorLayer());
			Application::Get().GetWindow().SetWindowIcon(Apex::Utils::LoadImage(FileSystem::GetFileIfExists("/assets/Apex-Game-Engine-32.png")));
		}
	};


	Application* CreateApplication()
	{
		return new ApexEditor();
	}

}
