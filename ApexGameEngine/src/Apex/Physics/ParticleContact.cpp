#include "apex_pch.h"
#include "ParticleContact.h"


namespace Apex::Physics {
	
	void ParticleContact::Resolve(real_t deltaTime)
	{
		ResolveVelocity(deltaTime);
		ResolvePenetration(deltaTime);
	}
	
	real_t ParticleContact::CalculateSeparationVelocity() const
	{
		vec3_t relativeVelocity = Particles[0]->Velocity - (Particles[1] ? Particles[1]->Velocity : vec3_t(0.0));
		
		return glm::dot(relativeVelocity, ContactNormal);
	}
	
	void ParticleContact::ResolveVelocity(real_t deltaTime)
	{
		real_t separatingVelocity = CalculateSeparationVelocity();
		APEX_CORE_DEBUG("Separating Velocity : {}", separatingVelocity);
		if (separatingVelocity > 0.0) return;
		
		vec3_t relConstAcc = Particles[0]->Acceleration - (Particles[1] ? Particles[1]->Acceleration : vec3_t(0.0));
		real_t sepVelFromAcc = glm::dot(relConstAcc, ContactNormal) * deltaTime; // Separation velocity due to constant acceleration
		
		// Remove separation velocity due to constant acceleration
		if (sepVelFromAcc < 0) {
			separatingVelocity -= sepVelFromAcc;
			separatingVelocity = std::min((real_t)0.0, separatingVelocity);
		}
		
		APEX_CORE_DEBUG("Separating Velocity (after correction) : {}", separatingVelocity);
		
		real_t newSepVelocity = -separatingVelocity * Restitution;
		real_t deltaVelocity = newSepVelocity - separatingVelocity;
		
		APEX_CORE_DEBUG("Delta Velocity : {}", deltaVelocity);
		
		real_t totalInvMass = Particles[0]->InverseMass + (Particles[1] ? Particles[1]->InverseMass : 0.0);
		if (totalInvMass <= 0.0) return; // All particles are immovable
		
		real_t impulseMag = deltaVelocity / totalInvMass;
		vec3_t impulsePerInvMass = ContactNormal * impulseMag;
		
		APEX_CORE_DEBUG("Impulse per inv Mass: {}, {}, {}", impulsePerInvMass.x, impulsePerInvMass.y, impulsePerInvMass.z);
		
		Particles[0]->Velocity += impulsePerInvMass * Particles[0]->InverseMass;
		if (Particles[1])
			Particles[1]->Velocity += (-impulsePerInvMass) * Particles[1]->InverseMass;
	}
	
	void ParticleContact::ResolvePenetration(real_t deltaTime)
	{
		APEX_CORE_DEBUG("Penetration: {}", Penetration);
		if (Penetration <= 0) return;
		
		real_t totalInvMass = Particles[0]->InverseMass + (Particles[1] ? Particles[1]->InverseMass : 0.0);
		if (totalInvMass <= 0.0) return; // All particles are immovable
		
		real_t distanceMag = Penetration / totalInvMass;
		vec3_t distancePerInvMass = ContactNormal * distanceMag;
		
		Particles[0]->Position += distancePerInvMass * Particles[0]->InverseMass;
		if (Particles[1])
			Particles[1]->Position += (-distancePerInvMass) * Particles[1]->InverseMass;
	}
	
	
	/*       Particle Contact Resolver     */
	
	void ParticleContactResolver::ResolveContacts(real_t deltaTime, std::vector<ParticleContact>& contacts, uint32_t usedContacts)
	{
		m_IterationsUsed = 0;
		
		while (m_IterationsUsed < m_MaxIterations) {
			// real_t minSep = std::numeric_limits<real_t>::max();
			real_t minSep = 0.0;
			uint32_t minSepIdx = usedContacts;
			for (auto i = 0; i < usedContacts; i++) {
				real_t sepVel = contacts[i].CalculateSeparationVelocity();
				
				// TODO : Remove second condition when ResolvePenetration is implemented
				if (sepVel < minSep || contacts[i].Penetration > 0) {
					minSep = sepVel;
					minSepIdx = i;
				}
			}
			
			if (minSepIdx >= usedContacts) break;
			
			// TODO : Change this to `minSepPtr->ResolveContacts` when ResolvePenetration is implemented
			contacts[minSepIdx].Resolve(deltaTime);
			
			m_IterationsUsed++;
		}
	}
	
	real_t ParticleContactResolver::CalculatePenetration(ParticleContact* contact)
	{
		// TODO
		return contact->Penetration;
	}
	
	void ParticleContactResolver::ResolvePenetration(real_t deltaTime, std::vector<ParticleContact>& contacts, uint32_t usedContacts)
	{
		m_IterationsUsed = 0;
		
		while (m_IterationsUsed < m_MaxIterations) {
			// real_t minSep = std::numeric_limits<real_t>::max();
			real_t maxPen = 0.0;
			auto maxPenIdx = usedContacts;
			for (auto i = 0; i < usedContacts; i++) {
				real_t penetration = CalculatePenetration(&contacts[i]);
				if (penetration > maxPen) {
					maxPen = penetration;
					maxPenIdx = i;
				}
			}
			
			if (maxPenIdx >= usedContacts) break;
			
			contacts[maxPenIdx].ResolvePenetration(deltaTime);
			
			m_IterationsUsed++;
		}
	}
	
	
	real_t ParticleLink::GetCurrentLength() const
	{
		vec3_t relativePos = m_Particles[0]->Position - m_Particles[1]->Position;
		return glm::length(relativePos);
	}
	
	
	uint32_t ParticleCable::AddContact(ParticleContact* contact, uint32_t limit) const
	{
		real_t length = GetCurrentLength();
		
		if (length < m_MaxLength) return 0; // No contact added if not extended
		
		contact->Particles[0] = m_Particles[0];
		contact->Particles[1] = m_Particles[1];
		
		vec3_t normal = m_Particles[1]->Position - m_Particles[0]->Position;
		contact->ContactNormal = glm::normalize(normal);
		
		contact->Penetration = length - m_MaxLength;
		contact->Restitution = m_Restitution;
		
		return 1;
	}
	
	real_t ParticleAnchoredCable::GetCurrentLength() const
	{
		vec3_t relativePos = m_Particles[0]->Position - *m_Anchor;
		return glm::length(relativePos);
	}
	
		
	uint32_t ParticleAnchoredCable::AddContact(ParticleContact* contact, uint32_t limit) const
	{
		real_t length = GetCurrentLength();
		
		if (length < m_MaxLength) return 0; // No contact added if not extended
		
		contact->Particles[0] = m_Particles[0];
		contact->Particles[1] = nullptr;
		
		
		vec3_t normal = *m_Anchor - m_Particles[0]->Position;
		contact->ContactNormal = glm::normalize(normal);
		
		contact->Penetration = length - m_MaxLength;
		contact->Restitution = m_Restitution;
		
		return 1;
	}
	
}
