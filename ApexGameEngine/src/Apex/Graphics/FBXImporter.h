#pragma once
#include "Apex/Core/ECS/Scene.h"

namespace Apex {

	class FBXImporter
	{
	public:
		static void Init();
		static void Shutdown();

		static void Import(const Ref<File>& file, const Ref<Scene>& scene = { nullptr });
		static Ref<Mesh> LoadMesh(const  Ref<File>& file, const std::string& nodeName = {},
		                          const Ref<Scene>& scene = {nullptr});
	};

}
