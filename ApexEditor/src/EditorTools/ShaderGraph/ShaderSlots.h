#pragma once

#include "Node.h"
#include "Apex/RenderPrimitives/Texture.h"

namespace Apex::EditorTools {

	struct SlotInt : public Slot
	{
		int value;

		SlotInt(const std::string& name)
			: Slot(name, DataType::INT)
		{
			value = 0;
		}

		// Inherited via Slot
		virtual std::string GetDefaultValue() override
		{
			return std::to_string(value);
		}
	};

	struct SlotFloat : public Slot
	{
		float value;
		
		SlotFloat(const std::string& name)
			: Slot(name, DataType::FLOAT)
		{
			value = 0.f;
		}

		// Inherited via Slot
		virtual std::string GetDefaultValue() override
		{
			return std::to_string(value);
		}
	};

	struct SlotVec2 : public Slot
	{
		glm::vec2 value;

		SlotVec2(const std::string& name)
			: Slot(name, DataType::VEC2)
		{
			value = { 0.f, 0.f };
		}

		// Inherited via Slot
		virtual std::string GetDefaultValue() override
		{
			return "vec2(" + std::to_string(value.r) + "," + std::to_string(value.g) + ")";
		}
	};

	struct SlotVec3 : public Slot
	{
		glm::vec3 value;

		SlotVec3(const std::string& name)
			: Slot(name, DataType::VEC3)
		{
			value = { 0.f, 0.f, 0.f };
		}

		// Inherited via Slot
		virtual std::string GetDefaultValue() override
		{
			return "vec3(" + std::to_string(value.r) + "," + std::to_string(value.g) + "," + std::to_string(value.b) + ")";
		}
	};

	struct SlotVec4 : public Slot
	{
		glm::vec4 value;

		SlotVec4(const std::string& name)
			: Slot(name, DataType::VEC4)
		{
			value = { 0.f, 0.f, 0.f, 0.f };
		}

		// Inherited via Slot
		virtual std::string GetDefaultValue() override
		{
			return "vec4(" + std::to_string(value.r) + "," + std::to_string(value.g) + "," + std::to_string(value.b) + "," + std::to_string(value.a) + ")";
		}
	};

	struct SlotTexture2D : public Slot
	{
		SlotTexture2D(const std::string& name)
			: Slot(name, DataType::TEX2D)
		{
		}
		
		// Inherited via Slot
		virtual std::string GetDefaultValue() override
		{
			return name;
		}
	};

}