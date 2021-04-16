#pragma once
#include "PhysicsCore.h"

namespace Apex::Physics {
	
	struct Particle; // Forward Declaration
	
	class ParticleForceGenerator
	{
	public:
		virtual void Update(real_t deltaTime, Particle* particle) = 0;
	};
	
	class ParticleForceRegistry
	{
	public:
		ParticleForceRegistry() = default;
		
		void Add(Particle* particle, ParticleForceGenerator* forceGenerator);
		void Remove(Particle* particle, ParticleForceGenerator* forceGenerator);
		void Clear();
		uint32_t Size() { return m_Registry.size(); }
		
		void UpdateForces(real_t deltaTime);
		
// 		decltype(auto) begin() { return m_Registry.begin(); }
// 		decltype(auto) end() { return m_Registry.end(); }
// 		
// 		decltype(auto) begin() const { return m_Registry.begin(); }
// 		decltype(auto) end() const { return m_Registry.end(); }
		
	private:
		struct PFEntry
		{
			Particle				*particle       = nullptr;
			ParticleForceGenerator	*forceGenerator = nullptr;
			
			PFEntry(Particle* particle, ParticleForceGenerator* forceGenerator)
				: particle(particle), forceGenerator(forceGenerator)
			{}
		};
		
		std::vector<PFEntry> m_Registry;
	};
	
	
	class GravityPFG : public ParticleForceGenerator
	{
	private:
		vec3_t m_AccGravity = { 0.0, -10.0, 0.0 };
		
	public:
		GravityPFG() = default;
		GravityPFG(const vec3_t& gravity) : m_AccGravity(gravity) {}
		virtual void Update(real_t deltaTime, Particle* particle) override;
	};
	
	class DragPFG : public ParticleForceGenerator
	{
	private:
		real_t m_K1, m_K2;
		
	public:
		DragPFG(real_t k1, real_t k2) : m_K1(k1), m_K2(k2) {}
		virtual void Update(real_t deltaTime, Particle* particle) override;
	};
	
	
	// Spring-Like Forces
	
	class SpringPFG : public ParticleForceGenerator
	{
	private:
		Particle* m_OtherParticle;
		real_t m_SpringConst;
		real_t m_RestLen;
		
	public:
		SpringPFG(real_t springConst, real_t restLen)
			: m_OtherParticle(nullptr), m_SpringConst(springConst), m_RestLen(restLen)
		{}
		
		SpringPFG(Particle* otherParticle, real_t springConst, real_t restLen)
			: m_OtherParticle(otherParticle), m_SpringConst(springConst), m_RestLen(restLen)
		{}
		
		void SetOtherParticle(Particle* particle)
		{
			m_OtherParticle = particle;
			APEX_CORE_DEBUG("Set Other Particle @ {}", (void*)particle);
		}
		
		virtual void Update(real_t deltaTime, Particle* particle) override;
		
	};
	
	class AnchoredSpringPFG : public ParticleForceGenerator
	{
	private:
		vec3_t* m_Anchor;
		real_t m_SpringConst;
		real_t m_RestLen;
		
	public:
		AnchoredSpringPFG(real_t springConst, real_t restLen)
			: m_Anchor(nullptr), m_SpringConst(springConst), m_RestLen(restLen)
		{}
		
		AnchoredSpringPFG(vec3_t* anchor, real_t springConst, real_t restLen)
			: m_Anchor(anchor), m_SpringConst(springConst), m_RestLen(restLen)
		{}
		
		void SetAnchor(vec3_t* anchor)
		{
			m_Anchor = anchor;
		}
		
		virtual void Update(real_t deltaTime, Particle* particle) override;
	};
	
}
