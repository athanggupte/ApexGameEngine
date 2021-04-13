#pragma once

#include "PhysicsCore.h"

namespace Apex {
	
	namespace Physics {
		
		struct Particle
		{
			vec3_t Position;
			vec3_t Velocity;
			vec3_t Acceleration; 	// Stores acceleration due to constant forces ex. gravity
			real_t InverseMass; 	// To avoid zero mass and allow infinite mass
			real_t Damping = 0.999; // To simulate simple air drag
			
			vec3_t Force = 0.0; 	// Total external force acting on the particle
			
			void SetMass(real_t mass) { InverseMass = 1.0 / mass; }
			void AddForce(const vec3_t& force) { Force += force; }
			void ClearForce() { Force = { 0.0, 0.0, 0.0 }; }
		};
		
		
		inline Particle& Update(real_t deltaTime, Particle& particle);
		inline vec3_t& Update(real_t deltaTime, vec3_t& position, const vec3_t& velocity);
		inline vec3_t& UpdatePosition(real_t deltaTime, vec3_t& position, const vec3_t& velocity);
		inline vec3_t& UpdateVelocity(real_t deltaTime, vec3_t& velocity, const vec3_t& acceleration);
		
	}
	
}
