#pragma once
#include "Apex/Core/ECS/Scene.h"

namespace Apex {

	class FBXImporter
	{
	public:
		static void Init();
		static void Shutdown();

		static void Import(const Ref<Scene>& scene, const std::filesystem::path& filepath);
		static void Load(const std::filesystem::path& filepath);
	};

}
