#pragma once

#include "Apex/Core/FileSystem/FileSystem.h"

namespace Apex {

	class AssetExplorer
	{
	public:
		AssetExplorer();

		void OnAttach();
		void SetContext(const fs::path& path);
		void OnImGuiRender();

	private:
		fs::path m_Context;
	};

}