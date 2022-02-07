#include "pch.h"

#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/ScriptFactory.h"
#include "Apex/Core/ECS/Components/BasicComponents.h"

using namespace Apex;

class SCRIPT_TEST_API MovementScript : public EntityScript
{
protected:
	void OnUpdate(Timestep ts) override
	{
		auto& transform = self().Transform();
		transform.translation.x = 10.f * glm::sin(Timer::GetElapsedTime());
		transform.translation.y = 10.f * glm::cos(Timer::GetElapsedTime());
	}
};

APEX_REGISTER_SCRIPT_FACTORY(MovementScript)
