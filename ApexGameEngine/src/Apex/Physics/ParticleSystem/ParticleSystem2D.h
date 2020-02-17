#pragma once

#include "Apex/RenderPrimitives/VertexArray.h"
#include "Apex/RenderPrimitives/Shader.h"

#include <glm/glm.hpp>
#include <vector>

namespace Apex {

	struct ParticleProps2D
	{
		glm::vec2 position;
		glm::vec2 velocity, velocityVariation;
		float rotationSpeed, rotationSpeedVariation;
		
		glm::vec4 colorBegin, colorEnd;
		float sizeBegin, sizeEnd, sizeVariation;
		
		float lifetime = 1.0f;
	};

	class ParticleSystem2D
	{
	public:
		//ParticleSystem2D();
		ParticleSystem2D(uint32_t batch_size = 5, uint32_t max_particles = 1000);

		void OnUpdate();
		void OnRender();

		void Emit(const ParticleProps2D& particleProps);

	private:
		struct Particle
		{
			glm::vec2 position;
			glm::vec2 velocity;
			float rotation;
			float rotationSpeed;
			
			glm::vec4 colorBegin, colorEnd;
			float sizeBegin, sizeEnd;
			
			float lifetime = 1.0f, lifeRemaining = 0.0f;

			bool active = false;
		};

		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex;
		uint32_t m_BatchSize;

		Ref<VertexArray> m_QuadVA;
		Ref<Shader> m_Shader;
	};

}