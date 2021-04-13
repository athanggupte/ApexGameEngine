#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	namespace Physics {
		
		#ifdef USE_DOUBLE_PRECISION
		using real_t = glm::float64; // double
		using vec2_t = glm::dvec2;
		using vec3_t = glm::dvec3;
		using vec4_t = glm::dvec4;
		using mat2_t = glm::dmat2;
		using mat3_t = glm::dmat3;
		using mat4_t = glm::dmat4;
		#else
		using real_t = glm::float32; // float
		using vec2_t = glm::vec2;
		using vec3_t = glm::vec3;
		using vec4_t = glm::vec4;
		using mat2_t = glm::mat2;
		using mat3_t = glm::mat3;
		using mat4_t = glm::mat4;
		#endif
		
		
		
		
	}
	
	
}
