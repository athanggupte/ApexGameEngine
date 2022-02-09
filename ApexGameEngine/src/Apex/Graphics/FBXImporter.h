#pragma once
#include "Apex/Core/ECS/Scene.h"

namespace Apex {

	class FBXImporter
	{
	public:
		static void Init();
		static void Shutdown();

		static void Import(const std::filesystem::path& filepath, const Ref<Scene>& scene = { nullptr });
		static Ref<Mesh> LoadMesh(const std::filesystem::path& filepath, const std::string& nodeName = {},
		                          const Ref<Scene>& scene = {nullptr});
	};

}
