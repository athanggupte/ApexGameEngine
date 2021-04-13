#include "apex_pch.h"
#include "ForceGenerator.h"

namespace Apex::Physics {
	
	void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* forceGenerator)
	{
		m_Registry.emplace_back(particle, forceGenerator);
	}
	
	void ParticleForceRegistry::Remove(Particle* particle, ParticleForceGenerator* forceGenerator)
	{
		auto& pfIt = m_Registry.begin();
		for (; pfIt != m_Registry.end(); ++pfIt) {
			if (pfentry.particle == particle && pfentry.forceGenerator == forceGenerator)
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
	
}
