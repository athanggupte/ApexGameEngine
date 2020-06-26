#pragma once

#include "Node.h"

namespace Apex::EditorTools {

	class ShaderGraph
	{
	public:
		ShaderGraph();

		virtual ~ShaderGraph()
		{
			// Delete all pointers
			for (auto nodePtr : m_Nodes)
				delete nodePtr;
			m_Nodes.clear();
		}

		bool RenderShaderGraph();

		std::string Parse();


	protected:
		virtual bool BeginCanvas(); // Draw grid, set canvas pos and zoom
		virtual void EndCanvas();	// Set canvas state for next frame, if any item selected/hovered take appropriate action
									// for eg. show tooltips or set last_active_slot

		virtual void CreateNode(uint32_t idx);
		virtual bool DrawNode(Node& node, std::vector<Node*>::const_iterator idx);
		virtual bool DrawSlot(Slot& slot);
		virtual void DrawSlotInputBox(Slot& slot);
		virtual bool RenderConnection(const ImVec2& input_pos, const ImVec2& output_pos, float thickness = 1.6f);

		inline bool HasPendingConnection() { return m_Canvas.activeSlot != nullptr; }

		void AddNode(Node* node, ImVec2 pos)
		{
			APEX_CORE_WARN(__FUNCTION__);
			m_Nodes.push_back(node);
			auto& curNode = m_Nodes.back();
			curNode->pos = pos; // set position
			curNode->id = m_Nodes.size(); // set id
			for (auto& inSlot : curNode->inputSlots) {
				inSlot->type = Slot::INPUT_SLOT;
				inSlot->parentNode = curNode;
			}
			for (auto& outSlot : curNode->outputSlots) {
				outSlot->type = Slot::OUTPUT_SLOT;
				outSlot->parentNode = curNode;
			}
		}

	private:
		std::function<Node* (uint32_t)> m_CreateNodeCallback;
		std::vector<std::string> m_NodeTypes;
		std::vector<std::string> m_DataTypes;

		NodeCanvas m_Canvas;
		std::vector<Node*> m_Nodes;
		ErrorState m_ErrorState;
	};

}