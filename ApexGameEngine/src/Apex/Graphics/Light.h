#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	struct DirectionalLight
	{
		glm::vec4 Direction;
		glm::vec4 Color;
		glm::vec4 Intensity;
	};
	
	struct PointLight
	{
		glm::vec4 Position;
		glm::vec4 Color;
		glm::vec4 Intensity;
		glm::vec4 Attenuation;
	};
	
}
