#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <imgui_internal.h>

namespace Apex::EditorTools {
	
	typedef struct Node Node;
	typedef struct Slot
	{
		enum SlotType
		{
			INPUT_SLOT = 0,
			OUTPUT_SLOT = 1
		};

		std::string name;
		SlotType type;
		uint32_t dataType;

		Node* parentNode;
		Slot* connectedSlot; // For input slots only

		float defaultValue = 0.f;
		// ImGui related attributes
		//ImGuiID id;
		ImVec2 pos;
	} Slot;

	struct Node
	{
		std::string name;

		std::vector<Slot*> inputSlots;
		std::vector<Slot*> outputSlots;

		// ImGui related attributes
		size_t id;
		ImVec2 pos, size;
		
		virtual std::string GetFunction() = 0;
		
		bool IsLeaf()
		{ 
			if (!inputSlots.empty())
				for (auto& inSlot : inputSlots)
					if (inSlot->connectedSlot != nullptr)
						return false;
			return true;
		}

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

	typedef struct Connection
	{
		//Node* inputNode = nullptr;
		Slot* inputSlot = nullptr;
		//Node* outputNode = nullptr;
		Slot* outputSlot = nullptr;

		bool IsPending() { return (/*inputNode && */inputSlot && /*outputNode &&*/ outputSlot) ? false : true; }
	} Connection;

	typedef struct NodeCanvas
	{
		ImVec2 offset = { 0.f, 0.f };
		float zoom = 1.f;

		Node* activeNode = nullptr;
		Slot* activeSlot = nullptr;
		ImVec2 activeSlotPos = { 0.f, 0.f };

		//Connection* activeConnection = nullptr;
	} NodeCanvas;

	typedef struct ErrorState
	{
		Node* errorNode = nullptr;
		Slot* errorSlot = nullptr;
	} ErrorState;

	bool BeginCanvas(NodeCanvas& canvas);
	void EndCanvas(NodeCanvas& canvas);
	bool DrawNode(Node& node, NodeCanvas& canvas);
	bool DrawSlot(Slot& slot, NodeCanvas& canvas);
	bool DrawConnection(Connection& connection, NodeCanvas& canvas);
}
