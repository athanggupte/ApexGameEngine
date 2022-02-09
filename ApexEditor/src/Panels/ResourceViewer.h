#pragma once

#include "Apex/Core/ResourceManager/ResourceManager.h"

namespace Apex {

	class ResourceViewer
	{
	public:
		ResourceViewer() = default;

		void OnAttach();
		void SetContext(ResourceManager& resourceManager);
		void OnImGuiRender();

	private:
		ResourceManager* m_ResourceManager = nullptr;
	};

}
