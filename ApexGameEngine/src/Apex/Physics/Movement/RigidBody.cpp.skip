#include "apex_pch.h"
#include "RigidBody.h"

#include "../PhysicsTimer/PhysicsTimer.h"

namespace Apex {

	void RigidBody2D::UpdateRigidBody()
	{
		auto dT = std::chrono::duration_cast<std::chrono::duration<float>>(PhysicsTimer::GetDeltaTime()).count();
		
		// Previous Values
		auto pos = m_Position.load();
		auto vel = m_Velocity.load();
		auto acc = m_Acceleration.load();

		// Update New Values
		vel += acc * dT;
		pos += vel * dT;

		if (pos.x >= 1.48f)
			vel.x = -glm::abs(vel.x);
		else if (pos.x <= -1.48f)
			vel.x = glm::abs(vel.x);
		if (pos.y >= 0.78f)
			vel.y = -glm::abs(vel.y);
		else if (pos.y <= -0.78f)
			vel.y = glm::abs(vel.y);

		m_Position = pos;
		m_Velocity = vel;
	}

}