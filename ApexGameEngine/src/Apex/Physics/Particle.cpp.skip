#include "apex_pch.h"
#include "Particle.h"

namespace Apex::Physics {
	
	Particle& Particle::Update(real_t deltaTime)
	{
// 		APEX_CORE_DEBUG("Particle ({}) ::\n\tPosition = {}, {}, {}\n\tVelocity = {}, {}, {}\n\tForce = {}, {}, {}",
// 						(void*)this, 
// 						Position.x, Position.y, Position.z,
// 						Velocity.x, Velocity.y, Velocity.z,
// 						Force.x, Force.y, Force.z
// 						);
		
		/* Aggregate acceleration from external and constant forces */
		vec3_t totalAcceleration = Acceleration + (InverseMass * Force);
		
		/* Update position and velocity based on Newton's Laws */
		UpdatePosition(deltaTime, Position, Velocity);
		UpdateVelocity(deltaTime, Velocity, totalAcceleration);
		
		/* Apply damping to velocity */
		Velocity *= glm::pow(Damping, deltaTime);
		
		/* Return changed particle */
		return *this;
	}
	
	vec3_t& UpdatePosition(real_t deltaTime, vec3_t& position, const vec3_t& velocity)
	{
		// Does not use the acceleration for now
		// Change only if deemed necessary
		position += velocity * deltaTime;
		return position;
	}
	
	vec3_t& UpdateVelocity(real_t deltaTime, vec3_t& velocity, const vec3_t& acceleration)
	{
		velocity += acceleration * deltaTime;
		return velocity;
	}
	
	
}
