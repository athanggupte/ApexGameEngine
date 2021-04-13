#include "apex_pch.h"
#include "Particle.h"

namespace Apex::Physics {
	
	Particle& Update(real_t deltaTime, Particle& particle)
	{
		/* Aggregate acceleration from external and constant forces */
		vec3_t totalAcceleration = particle.Acceleration + (particle.InverseMass * particle.Force);
		
		/* Update position and velocity based on Newton's Laws */
		Update(deltaTime, particle.Position, particle.Velocity, totalAcceleration);
		
		/* Apply damping to velocity */
		particle.Velocity *= glm::pow(particle.Damping, deltaTime);
		
		/* Reset/Clear accumulated forces */
		particle.ClearForce();
	}
	
	vec3_t& Update(real_t deltaTime, vec3_t& position, vec3_t& velocity, const vec3_t& acceleration)
	{
		UpdatePosition(deltaTime, position, velocity, acceleration);
		UpdateVelocity(deltaTime, velocity, acceleration);
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
