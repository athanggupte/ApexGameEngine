#pragma once

#include "Node.h"
#include "ConnectionTree.h"

namespace Apex::EditorTools {

	class NodeGraph
	{
	public:
		NodeGraph() = delete;

		NodeGraph(std::vector<std::string>& nodeTypes, std::function<Node* (uint32_t)> callback);

		virtual ~NodeGraph()
		{
			// Delete all pointers
			for (auto nodePtr : m_Nodes)
				delete nodePtr;
			m_Nodes.clear();
		}

		bool RenderNodeGraph();

		std::string Parse();


	protected:
		virtual bool BeginCanvas(); // Draw grid, set canvas pos and zoom
		virtual void EndCanvas();	// Set canvas state for next frame, if any item selected/hovered take appropriate action
									// for eg. show tooltips or set last_active_slot

		virtual bool DrawNode(Node& node, std::vector<Node*>::const_iterator idx);
		virtual bool DrawSlot(Slot& slot);
		virtual bool DrawConnection(Connection& connection);
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
		//std::function<bool(uint32_t, uint32_t)> m_TypeConformingCallback; // Use for using dissimilar but conforming data types together eg. float and vec3
		std::vector<std::string> m_NodeTypes;
		std::vector<std::string> m_DataTypes;
		
		NodeCanvas m_Canvas;
		std::vector<Node*> m_Nodes;
		ErrorState m_ErrorState;
	};

}