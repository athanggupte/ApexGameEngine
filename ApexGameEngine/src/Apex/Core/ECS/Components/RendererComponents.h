#pragma once
#include "Apex/Graphics/Mesh.h"

namespace Apex {
	
	struct MeshRendererComponent
	{
		Resource<Mesh> mesh;
		Resource<Material> material;
		bool castShadows = true;

		COMPONENT_DEFAULT_CTR(MeshRendererComponent);

		MeshRendererComponent(const Resource<Mesh>& _mesh, const Resource<Material>& _material)
			: mesh(_mesh), material(_material) {}
	};

	enum class LightType
	{
		DirectionalLight,
		PointLight,
		SpotLight,
		AreaLight,

		_COUNT
	};

	constexpr const char* LightTypeString(LightType type)
	{
		switch (type)
		{
		case LightType::DirectionalLight: return "Directional";
		case LightType::PointLight: return "Point";
		case LightType::SpotLight: return "Spot";
		case LightType::AreaLight: return "Area";
		default: return "UNKNOWN";
		}
	}

	struct LightComponent
	{
		LightType type = LightType::PointLight;

		glm::vec4 color { 1.f };
		float intensity = 1.f;

		float attenuationConstant = 1.f;
		float attenuationLinear;
		float attenuationQuadratic;

		float radius = 10.f;

		bool enableShadows = true;

		COMPONENT_DEFAULT_CTR(LightComponent);

	};

}
