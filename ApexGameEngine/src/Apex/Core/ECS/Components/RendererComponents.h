#pragma once
#include "Apex/Graphics/Mesh.h"

namespace Apex
{

	struct MeshRendererComponent
	{
		Ref<Mesh> mesh = nullptr;
		Ref<Shader> shader = nullptr;

		COMPONENT_DEFAULT_CTR(MeshRendererComponent);

		MeshRendererComponent(const Ref<Mesh>& _mesh, const Ref<Shader>& _shader)
			: mesh(_mesh), shader(_shader) {}
	};


}
