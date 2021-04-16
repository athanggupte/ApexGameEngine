#pragma once
#include "Particle.h"
#include "ParticleForce.h"
#include "ParticleContact.h"

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Entity.h"

namespace Apex {
	
	class PhysicsSystem
	{
	public:
		static void SetScene(Scene* scene);
		
		static Physics::Particle* GetNewParticle();
		static void AttachForceGenerator(Entity& entity, Physics::ParticleForceGenerator* fg);
		static void AddContactGenerator(Physics::ParticleContactGenerator* cg);
		
		static void StartFrame();
		static uint32_t GenerateContacts();
		static void UpdateParticles(Physics::real_t deltaTime);
		static void Update(Physics::real_t deltaTime);
		
		static uint32_t GetNumContactsInFrame();
	};
	
	
	struct ParticleComponent
	{
		Physics::Particle* ParticlePtr;
		
		ParticleComponent() = default;
		ParticleComponent(const ParticleComponent&) = default;
		
		ParticleComponent(Physics::Particle* particlePtr)
			: ParticlePtr(particlePtr) {}
			
		ParticleComponent(Physics::Particle& particle)
			: ParticlePtr(PhysicsSystem::GetNewParticle())
		{
			*ParticlePtr = particle;
		}
	};
	
	
// 	template<>
// 	inline void Entity::AddComponent<ParticleComponent>(const Physics::Particle& dummyParticle)
// 	{
// 		auto particlePtr = PhysicsSystem::GetNewParticle();
// 		*particlePtr = dummyParticle;
// 		m_Registry->emplace<ParticleComponent>(m_EntityId, particlePtr);
// 	}
	
}
