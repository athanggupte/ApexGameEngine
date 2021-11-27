#pragma once
#include "Apex/Graphics/Mesh.h"

namespace Apex {
	
	struct MeshRendererComponent
	{
		Resource<Mesh> mesh;
		Resource<Material> material;

		COMPONENT_DEFAULT_CTR(MeshRendererComponent);

		MeshRendererComponent(const Resource<Mesh>& _mesh, const Resource<Material>& _material)
			: mesh(_mesh), material(_material) {}
	};

}
