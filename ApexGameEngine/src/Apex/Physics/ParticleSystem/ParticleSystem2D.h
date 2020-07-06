#pragma once

#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"

#include <glm/glm.hpp>
#include <vector>

namespace Apex {

	struct ParticleProps2D
	{
		glm::vec2 position;
		glm::vec2 velocity, velocityVariation;
		glm::vec2 acceleration;
		float rotation, rotationSpeed, rotationSpeedVariation;
		
		glm::vec4 colorBegin, colorEnd;
		glm::vec2 sizeBegin, sizeEnd, sizeVariation;

		bool useTexture = false;

		uint32_t textureNumRows = 1;
		uint32_t textureIndex = 0;

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
			glm::vec2 acceleration;
			float rotation;
			float rotationSpeed;
			
			glm::vec4 colorBegin, colorEnd;
			glm::vec2 sizeBegin, sizeEnd;
			
			bool useTexture = false;

			uint32_t textureNumRows = 1;
			uint32_t textureIndex = 0;
			
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