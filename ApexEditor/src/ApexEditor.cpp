#include "apexed_pch.h"
#include <Apex.h>
#include "Apex/StartUp.h"

#include "EditorLayer.h"
#include "Apex/Graphics/FBXImporter.h"
#include "Apex/Utils/Utils.h"

namespace Apex {

	class ApexEditor : public Application
	{
	public:
		ApexEditor()
			: Application(WindowProps("ApexEditor", 1600u, 900u))
		{
			FBXImporter::Init();
			PushLayer(new EditorLayer());
			this->GetWindow().SetWindowIcon(Apex::Utils::LoadImage_internal(APEX_INSTALL_LOCATION "/assets/Apex-Game-Engine-32.png"));
		}

		~ApexEditor() override
		{
			FBXImporter::Shutdown();
		}
	};


	Application* CreateApplication()
	{
		return new ApexEditor();
	}

}
