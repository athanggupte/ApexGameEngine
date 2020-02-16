#include "apex_pch.h"
#include "ParticleSystem2D.h"

#include "Apex/Utils/Timer.h"
#include "Apex/Utils/Random.h"

#include <glm/gtc/constants.hpp>

namespace Apex {
	
	ParticleSystem2D::ParticleSystem2D(uint32_t max_particles, uint32_t batch_size)
		: m_BatchSize(batch_size), m_PoolIndex(max_particles - 1)
	{
		m_ParticlePool.resize(max_particles);
	}

	void ParticleSystem2D::OnUpdate()
	{
		for(auto& particle : m_ParticlePool) {
			if (!particle.active)
				continue;

			if (particle.lifeRemaining <= 0.0f) {
				particle.active = false;
				continue;
			}

			particle.lifeRemaining -= Timer::GetSeconds();
			particle.position += particle.velocity * Timer::GetSeconds();
			particle.rotation += particle.rotationSpeed * Timer::GetSeconds();
		}
	}

	void ParticleSystem2D::OnRender()
	{

	}

	void ParticleSystem2D::Emit(const ParticleProps2D & particleProps)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.active = true;
		particle.position = particleProps.position;
		particle.rotation = Random::Float() * 2.0f * glm::pi<float>();

		// Velocity
		particle.velocity = particleProps.velocity;
		particle.velocity.x += particleProps.velocityVariation.x + (Random::Float() - 0.5f);
		particle.velocity.y += particleProps.velocityVariation.y + (Random::Float() - 0.5f);

		// Color
		particle.colorBegin = particleProps.colorBegin;
		particle.colorEnd = particleProps.colorEnd;

		// Lifetime
		particle.lifetime = particleProps.lifetime;
		particle.lifeRemaining = particleProps.lifetime;

		// Size
		particle.sizeBegin = particleProps.sizeBegin + particleProps.sizeVariation * (Random::Float() - 0.5f);
		particle.sizeEnd = particleProps.sizeEnd - particleProps.sizeVariation * (Random::Float() - 0.5f);

		m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
	}

}
