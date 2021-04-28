#pragma once
#include "Particle.h"

namespace Apex::Physics {
	
	struct Derivative
	{
		vec3_t Velocityx;
		vec3_t Acceleration;
	};
	
	
	// Abstract class / interface
	class ParticleConstraint
	{
	public:
		virtual Derivative GetDerivative(real_t deltaTime, Particle* particle) = 0;
	};


	class AnchoredRod : public ParticleConstraint
	{
	private:
		vec3_t* m_Anchor;
		real_t m_Length;
	
	public:
		virtual Derivative GetDerivative(real_t deltaTime, Particle* particle) override;
		
	};
	
	
}
