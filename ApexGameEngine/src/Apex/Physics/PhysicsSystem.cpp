#include "apex_pch.h"
#include "PhysicsSystem.h"

// #include "Particle.h"
// #include "ParticleForce.h"
// #include "ParticleContact.h"

//#include "Apex/Core/ECS/Scene.h"

namespace Apex {
	
	using real_t = Physics::real_t;
	using vec3_t = Physics::vec3_t;
	
	struct PhysicsState
	{
		constexpr static uint32_t MAX_CONTACTS = 100;
// 		constexpr static uint32_t MAX_PARTICLES = 10000;
		constexpr static uint32_t MAX_ITERATIONS = 2 * MAX_CONTACTS;
		
		constexpr static uint32_t PARTICLE_POOL_SIZE = 256;
		
		//Scene* scene;
		Physics::ParticleForceRegistry forceRegistry;
		std::vector<Physics::Particle> particles{ PARTICLE_POOL_SIZE };
		std::vector<Physics::ParticleContact> contacts{ MAX_CONTACTS };
		std::vector<Physics::ParticleContactGenerator*> contactGenerators;
		Physics::ParticleContactResolver contactResolver{ 1 };
		
		uint32_t numParticles = 0;
		uint32_t numUsedContacts = 0;
	};
	
	static PhysicsState s_State;

	void PhysicsSystem::SetScene(Scene* scene)
	{
		//s_State.scene = scene;
	}
	
	Physics::Particle* PhysicsSystem::GetNewParticle()
	{
		// TODO : Use Pool allocator
		if (s_State.numParticles < s_State.particles.size())
			return &s_State.particles[s_State.numParticles++];
		
		return nullptr;
	}
	
	void PhysicsSystem::AttachForceGenerator(Entity& entity, Physics::ParticleForceGenerator* fg)
	{
		APEX_CORE_ASSERT(entity.HasComponent<ParticleComponent>(), "Entity must have particle component to attach Force!");
		auto particle = entity.GetComponents<ParticleComponent>().ParticlePtr;
		s_State.forceRegistry.Add(particle, fg);
		APEX_CORE_DEBUG("Number of entries in Force Registry : {}", s_State.forceRegistry.Size());
	}
	
	void PhysicsSystem::AddContactGenerator(Physics::ParticleContactGenerator* cg)
	{
		s_State.contactGenerators.emplace_back(cg);
	}
	
	void PhysicsSystem::StartFrame()
	{
		// Clear all accumulators
		for (auto i=0; i<s_State.numParticles; i++)
			s_State.particles[i].ClearForce();
	}
	
	uint32_t PhysicsSystem::GenerateContacts()
	{
		uint32_t limit = PhysicsState::MAX_CONTACTS;
		auto contactIter = s_State.contacts.begin();
		
		for (auto contactGenIt = s_State.contactGenerators.begin(); contactGenIt  != s_State.contactGenerators.end(); ++contactGenIt) {
			auto used = (*contactGenIt)->AddContact(&(*contactIter), limit);
			limit -= used;
			contactIter += used;
			
			if (limit <= 0) break;
		}
		
		return PhysicsState::MAX_CONTACTS - limit;
	}
	
	void PhysicsSystem::UpdateParticles(real_t deltaTime)
	{
		for (auto i=0; i<s_State.numParticles; i++)
			s_State.particles[i].Update(deltaTime);
	}
	
	void PhysicsSystem::Update(real_t deltaTime)
	{
		// Update Forces
		s_State.forceRegistry.UpdateForces(deltaTime);
		
		// Update Particles
		UpdateParticles(deltaTime);
		
		// Generate Contacts
		s_State.numUsedContacts = GenerateContacts();
		
		// Resolve Contacts
		if (s_State.numUsedContacts) {
			s_State.contactResolver.SetMaxIterations(s_State.numUsedContacts * 2);
			s_State.contactResolver.ResolveContacts(deltaTime, s_State.contacts, s_State.numUsedContacts);
		}
	}
	
	uint32_t PhysicsSystem::GetNumContactsInFrame()
	{
		return s_State.numUsedContacts;
	}
	
}
