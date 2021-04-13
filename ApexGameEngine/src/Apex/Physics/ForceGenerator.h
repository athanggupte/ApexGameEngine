#pragma once
#include "Particle.h"

namespace Apex::Physics {
	
	class ParticleForceGenerator
	{
	public:
		void Update(real_t deltaTime, Particle* particle) = 0;
	};
	
	class ParticleForceRegistry
	{
	public:
		void Add(Particle* particle, ParticleForceGenerator* forceGenerator);
		void Remove(Particle* particle, ParticleForceGenerator* forceGenerator);
		void Clear();
		
		void UpdateForces();
		
// 		decltype(auto) begin() { return m_Registry.begin(); }
// 		decltype(auto) end() { return m_Registry.end(); }
// 		
// 		decltype(auto) begin() const { return m_Registry.begin(); }
// 		decltype(auto) end() const { return m_Registry.end(); }
		
	private:
		struct PFEntry
		{
			Particle				*particle;
			ParticleForceGenerator	*forceGenerator;
		};
		
		std::vector<PFEntry> m_Registry;
	};
	
}
