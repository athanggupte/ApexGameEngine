#include <Apex.h>

#include "EditorLayer.h"

namespace Apex {

	class ApexEditor : public Application
	{
	public:
		ApexEditor()
		{
			PushLayer(new EditorLayer());
		}
	};


	Application* CreateApplication()
	{
		return new ApexEditor();
	}

}
