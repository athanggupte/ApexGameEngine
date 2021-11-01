#pragma once
#include "Apex/Graphics/Mesh.h"

namespace Apex {

	// TODO: Merge MeshRendererComponent and ModelRendererComponent into one!!!!

	struct MeshRendererComponent
	{
		Handle mesh = 0;
		Handle material = 0;

		COMPONENT_DEFAULT_CTR(MeshRendererComponent);

		MeshRendererComponent(Handle _mesh, Handle _material)
			: mesh(_mesh), material(_material) {}
	};

}
