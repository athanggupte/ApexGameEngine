#include "apex_pch.h"
#include "ParticleForce.h"

#include "Particle.h"

namespace Apex::Physics {
	
	void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* forceGenerator)
	{
		APEX_CORE_DEBUG("Added Force to Particle @ {}", (void*)particle);
		m_Registry.emplace_back(particle, forceGenerator);
	}
	
	void ParticleForceRegistry::Remove(Particle* particle, ParticleForceGenerator* forceGenerator)
	{
		auto pfIt = m_Registry.begin();
		for (; pfIt != m_Registry.end(); ++pfIt) {
			if (pfIt->particle == particle && pfIt->forceGenerator == forceGenerator)
				break;
		}
		if (pfIt != m_Registry.end()) {
			std::swap(*pfIt, m_Registry.back());
			m_Registry.pop_back();
		}
	}
	
	void ParticleForceRegistry::Clear()
	{
		m_Registry.clear();
	}
	
	void ParticleForceRegistry::UpdateForces(real_t deltaTime)
	{
		for (auto& pfentry : m_Registry) {
			pfentry.forceGenerator->Update(deltaTime, pfentry.particle);
		}
	}
	
	
	void GravityPFG::Update(real_t deltaTime, Particle* particle)
	{
		if (!particle->InverseMass) return;
		
		particle->AddForce(m_AccGravity / particle->InverseMass);
	}

	
	void DragPFG::Update(real_t deltaTime, Particle* particle)
	{
		/* Drag Force : f_drag = -V' * (k1|V| + k2|V|^2)
		 *                            ^^^^^^^^^^^^^^^ <- Drag coefficient
		 *                       ^^^ <- Unit vector in dir of velocity
		 */
		vec3_t force = particle->Velocity;
		
		real_t coeff = glm::length(force);
		coeff = (m_K1 * coeff) + (m_K2 * coeff * coeff);
		
		force = glm::normalize(force);
		force *= -coeff;
		
		particle->AddForce(force);
	}
	
	
	void SpringPFG::Update(real_t deltaTime, Particle* particle)
	{
		/* Spring Force : f_spring = - K * (|D| - |L0|) * D'
		 *
		 */
		vec3_t force = particle->Position - m_OtherParticle->Position; // D
		
		real_t magnitude = glm::length(force); // |D|
		magnitude = magnitude - m_RestLen; // |D| - |L0|
		magnitude *= m_SpringConst; // K * (|D| - |L0|)
		
		force = glm::normalize(force); // D'
		force *= -magnitude; // - K * (|D| - |L0|) * D'
		
		particle->AddForce(force);
	}
	
	void AnchoredSpringPFG::Update(real_t deltaTime, Particle* particle)
	{
		/* Spring Force : f_spring = - K * (|D| - |L0|) * D'
		 *
		 */
		vec3_t force = particle->Position - *m_Anchor; // D
		
		real_t magnitude = glm::length(force); // |D|
		magnitude = magnitude - m_RestLen; // |D| - |L0|
		magnitude *= m_SpringConst; // K * (|D| - |L0|)
		
		force = glm::normalize(force); // D'
		force *= -magnitude; // - K * (|D| - |L0|) * D'
		
		particle->AddForce(force);
	}
	
}
