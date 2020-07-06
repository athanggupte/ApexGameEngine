#pragma once

#include "Node.h"

namespace Apex::EditorTools {

	struct SlotInt : public Slot
	{
		int value;
	};

	struct SlotFloat : public Slot
	{
		float value;
	};

	struct SlotVec2 : public Slot
	{
		glm::vec2 value;
	};

	struct SlotVec3 : public Slot
	{
		glm::vec3 value;
	};

}