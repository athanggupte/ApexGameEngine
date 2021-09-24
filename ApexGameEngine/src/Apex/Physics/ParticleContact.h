#pragma once
#include "Particle.h"

namespace Apex::Physics {
	
	/*
	* Describes a contact/collision between two particles
	*/
	struct ParticleContact
	{
		Particle* Particles[2] = { nullptr, nullptr };
		vec3_t ContactNormal;
		real_t Restitution = 1.0;
		real_t Penetration = 0.0;
		
		void Resolve(real_t deltaTime);
		real_t CalculateSeparationVelocity() const;
		void ResolveVelocity(real_t deltaTime);
		void ResolvePenetration(real_t deltaTime);
	};
	
	
	class ParticleContactResolver
	{
	private:
		uint32_t m_MaxIterations;
		uint32_t m_IterationsUsed;
		
	public:
		ParticleContactResolver(uint32_t maxIters) : m_MaxIterations(maxIters) {}
		
		void SetMaxIterations(uint32_t maxIters) { m_MaxIterations = maxIters; }
		
		real_t CalculatePenetration(ParticleContact* contact);
		
		void ResolveContacts(real_t deltaTime, std::vector<ParticleContact>& contacts, uint32_t usedContacts);
		void ResolvePenetration(real_t deltaTime, std::vector<ParticleContact>& contacts, uint32_t usedContacts);
	
	protected:
		// TODO
		void CalculatePenetration();
	};
	
	
	class ParticleContactGenerator
	{
	public:
		/*
		 * @param limit Defines max number of contacts that can be added
		 * @return Number of contacts added
		 */
		virtual uint32_t AddContact(ParticleContact* contact, uint32_t limit) const = 0;
	};
	
	
	
	// Cables
	
	class ParticleLink : public ParticleContactGenerator
	{
	public:
		Particle* m_Particles[2] = { nullptr, nullptr };
		
	public:
		virtual uint32_t AddContact(ParticleContact* contact, uint32_t limit) const = 0;
		
	protected:
		virtual real_t GetCurrentLength() const;
	};
	
	class ParticleCable : public ParticleLink
	{
	public:
		real_t m_MaxLength;
		real_t m_Restitution;
	
	public:
		ParticleCable(real_t restitution, real_t maxLen)
			: m_MaxLength(maxLen), m_Restitution(restitution) {}
			
		void SetParticles(Particle* particle1, Particle* particle2)
		{
			m_Particles[0] = particle1;
			m_Particles[1] = particle2;
		}
		
		virtual uint32_t AddContact(ParticleContact* contact, uint32_t limit) const override;
	};
	
	class ParticleAnchoredCable : public ParticleLink
	{
	public:
		vec3_t* m_Anchor;
		real_t m_MaxLength;
		real_t m_Restitution;
	
		virtual real_t GetCurrentLength() const override;
		
	public:
		ParticleAnchoredCable(real_t restitution, real_t maxLen)
			: m_MaxLength(maxLen), m_Restitution(restitution) {}
		
		void SetParticle(Particle* particle)
		{
			m_Particles[0] = particle;
		}
		
		void SetAnchor(vec3_t* anchor)
		{
			m_Anchor = anchor;
		}
		
		virtual uint32_t AddContact(ParticleContact* contact, uint32_t limit) const override;
	};
	
}
