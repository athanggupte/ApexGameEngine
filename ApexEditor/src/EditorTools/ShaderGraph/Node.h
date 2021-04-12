#pragma once

#include <glm/glm.hpp>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <imgui_internal.h>

#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/ComputeShader/ComputeShader.h"

namespace Apex::EditorTools {
	
	enum DataType
	{
		BOOL, INT, FLOAT,
		IVEC2, IVEC3, IVEC4,
		VEC2, VEC3, VEC4,
		MAT2, MAT3, MAT4,
		TEX2D, TEX3D
	};

	typedef struct Node Node;
	typedef struct Slot
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

		// ImGui related attributes
		ImVec2 pos;

		Slot(const std::string& name, DataType dataType)
		{
			this->name = name;
			this->dataType = dataType;
			this->parentNode = nullptr;
			this->connectedSlot = nullptr;
		}

		virtual std::string GetDefaultValue() = 0;
	} Slot;

	struct Node
	{
		std::string name;

		std::vector<Slot*> inputSlots;
		std::vector<Slot*> outputSlots;

		// ImGui related attributes
		size_t id;
		ImVec2 pos, size;
		
		Ref<ComputeShader> previewCompute;
		Ref<Texture2D> previewTexture;

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

	typedef struct NodeCanvas
	{
		ImVec2 offset = { 0.f, 0.f };
		float zoom = 1.f;

		Node* activeNode = nullptr;
		Slot* activeSlot = nullptr;
	} NodeCanvas;

	typedef struct ErrorState
	{
		Node* errorNode = nullptr;
		Slot* errorSlot = nullptr;
	} ErrorState;

}
