#pragma once

#include "PhysicsCore.h"

namespace Apex {
	
	namespace Physics {
		
		/* 
		 * Describes a point mass
		 */
		struct Particle
		{
			vec3_t Position = vec3_t(0.0);
			vec3_t Velocity = vec3_t(0.0);
			vec3_t Acceleration = vec3_t(0.0); 	// Stores acceleration due to constant forces ex. gravity
			real_t InverseMass = 0.0; 			// To avoid zero mass and allow infinite mass
			real_t Damping = 0.999; 			// To simulate simple air drag
			
			vec3_t Force = vec3_t(0.0); 	// Total external force acting on the particle
			
			void SetMass(real_t mass) { InverseMass = 1.0 / mass; }
			void AddForce(const vec3_t& force) { Force += force; }
			void ClearForce() { Force = vec3_t(0.0); }
			
			Particle& Update(real_t deltaTime);
			
			// ~Particle() { APEX_CORE_WARN("Particle @ {} destroyed", (void*)this); }
		};
		
		
		void Update(real_t deltaTime, vec3_t& position, vec3_t& velocity, const vec3_t& acceleration);
		vec3_t& UpdatePosition(real_t deltaTime, vec3_t& position, const vec3_t& velocity);
		vec3_t& UpdateVelocity(real_t deltaTime, vec3_t& velocity, const vec3_t& acceleration);
		
	}
	
}
