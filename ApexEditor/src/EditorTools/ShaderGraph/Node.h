#pragma once

#include <glm/glm.hpp>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <imgui_internal.h>

namespace Apex::EditorTools {
	
	enum DataType
	{
		BOOL, INT, FLOAT,
		IVEC2, IVEC3, IVEC4,
		VEC2, VEC3, VEC4,
		MAT2, MAT3, MAT4,
		TEX2D, TEX3D
	};

	struct Node;
	struct Slot
	{
		enum SlotType
		{
			INPUT_SLOT = 0,
			OUTPUT_SLOT = 1
		};

		std::string name;
		DataType dataType;
		SlotType type;

		Node* parentNode;
		Slot* connectedSlot; // For input slots only

		//std::string defaultValue;

		// ImGui related attributes
		//ImGuiID id;
		ImVec2 pos;
	};

	struct Node
	{
		std::string name;

		std::vector<Slot*> inputSlots;
		std::vector<Slot*> outputSlots;

		// ImGui related attributes
		size_t id;
		ImVec2 pos, size;
		
		virtual std::string GetFunction() = 0;

		virtual ~Node()
		{
			for (auto slotPtr : inputSlots)
				delete slotPtr;
			inputSlots.clear();
			for (auto slotPtr : outputSlots)
				delete slotPtr;
			outputSlots.clear();
		}
	};

	struct NodeCanvas
	{
		ImVec2 offset = { 0.f, 0.f };
		float zoom = 1.f;

		Node* activeNode = nullptr;
		Slot* activeSlot = nullptr;
	};

	struct ErrorState
	{
		Node* errorNode = nullptr;
		Slot* errorSlot = nullptr;
	};

}