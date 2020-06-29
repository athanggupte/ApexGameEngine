#include <apex_pch.h>
#include "ShaderGraph.h"

#include "ShaderSlots.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <imgui_internal.h>

#include <regex>

namespace Apex::EditorTools {

	//#define NODE_SIZE_X 220.f
#define TITLE_SIZE_Y 21.f * m_Canvas.zoom
#define CIRCLE_RADIUS 4.9f * m_Canvas.zoom

	ShaderGraph::ShaderGraph()
	{
		m_Nodes.reserve(8);
		m_NodeTypes.push_back("Output");
		m_NodeTypes.push_back("Test");
		AddNode(0, { 200.f, 200.f });
	}

	bool ShaderGraph::RenderShaderGraph()
	{
		BeginCanvas();
		std::vector<Node*>::iterator it = m_Nodes.begin();
		for (auto& node : m_Nodes)
			DrawNode(*node, it++);
		EndCanvas();
		return true;
	}

	std::string ShaderGraph::Parse()
	{
		std::string program;
		std::map<Node*, bool> visited_nodes;

		// Visit nodes using BFS starting from Root node i.e. m_Nodes[0]
		// by following the links to OUTPUT_SLOTs from each INPUT_SLOT

		std::queue<Node*> nodeQue;
		std::stack<Node*> nodeStack;
		nodeQue.push(m_Nodes[0]);
		visited_nodes[m_Nodes[0]] = true;

		while (!nodeQue.empty()) {
			auto curNode = nodeQue.front();
			nodeQue.pop();
			nodeStack.push(curNode);

			for (auto inSlot : curNode->inputSlots) {
				if (inSlot->connectedSlot) {
					if (!visited_nodes[inSlot->connectedSlot->parentNode])
						nodeQue.push(inSlot->connectedSlot->parentNode);
				}
			}
		}

		visited_nodes.clear();

		while (!nodeStack.empty()) {
			auto curNode = nodeStack.top();
			nodeStack.pop();
			if (!visited_nodes[curNode]) {
				// Assign values to the inputs
				for (auto inSlot : curNode->inputSlots) {
					auto value = (inSlot->connectedSlot == nullptr) ? inSlot->GetDefaultValue() :
						(inSlot->connectedSlot->name + std::to_string(inSlot->connectedSlot->parentNode->id));
					program += inSlot->name + std::to_string(curNode->id) + " = " + value + "\n";
				}
				// Execute the function
				auto operation = curNode->GetFunction();
				operation = std::regex_replace(operation, std::regex("#"), std::to_string(curNode->id));
				program += operation + "\n";
				visited_nodes[curNode] = true;
			}
		}

		return program;
	}

	////////////////////////////////////////////////////////////////
	////////////////////       Canvas           ////////////////////
	////////////////////////////////////////////////////////////////

	bool ShaderGraph::BeginCanvas()
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImGuiWindow* w = ImGui::GetCurrentWindow();
		ImGuiIO& io = ImGui::GetIO();

		auto font = io.Fonts->Fonts[0];
		ImGui::PushFont(font);
		auto curPadding = style.FramePadding;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { curPadding.x, curPadding.y * m_Canvas.zoom });
		auto curItemSpacing = style.ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { curItemSpacing.x, curItemSpacing.y * m_Canvas.zoom });

		ImGui::PushID(&m_Canvas);
		if (!ImGui::ItemAdd(w->ContentRegionRect, ImGui::GetID("canvas"))) {
			ImGui::PopID();
			return false;
		}

		/* Get Mouse Interactions */
		if (ImGui::IsWindowHovered()) {
			/*if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				ImGui::BeginP
			}*/
			if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup("Add_Node");
			}
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
				m_Canvas.offset += io.MouseDelta;

			if (io.MouseWheel != 0) {
				ImVec2 mouseRel = ImVec2{ ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y };
				float prevZoom = m_Canvas.zoom;
				m_Canvas.zoom = ImClamp(m_Canvas.zoom + io.MouseWheel * m_Canvas.zoom / 16.f, 0.5f, 1.5f);
				float zoomFactor = (prevZoom - m_Canvas.zoom) / prevZoom;
				m_Canvas.offset += (mouseRel - m_Canvas.offset) * zoomFactor;
				font->Scale = m_Canvas.zoom;
				//ImGui::SetWindowFontScale(m_Canvas.zoom);
			}
		}

		/* Draw Grid */
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		//const ImVec2 windowMin = w->ContentRegionRect.Min;
		//const ImVec2 windowMax = w->ContentRegionRect.Max;
		const float gridSize = 64.f * m_Canvas.zoom;
		//auto gridColor = IM_COL32(150, 150, 150, 150);
		auto gridColor = ImColor(style.Colors[ImGuiCol_Separator]);

		for (float x = fmodf(m_Canvas.offset.x, gridSize); x < size.x; x += gridSize) {
			//drawList->AddLine(ImVec2(x, windowMin.y), ImVec2(x, windowMax.y), gridColor);
			drawList->AddLine(ImVec2(x, 0) + pos, ImVec2(x, size.y) + pos, gridColor, 2.2f);
		}
		for (float y = fmodf(m_Canvas.offset.y, gridSize); y < size.y; y += gridSize) {
			//drawList->AddLine(ImVec2(windowMin.x, y), ImVec2(windowMax.x, y), gridColor);
			drawList->AddLine(ImVec2(0, y) + pos, ImVec2(size.x, y) + pos, gridColor, 2.2f);
		}

		//ImGui::SetWindowFontScale(m_Canvas.zoom);

		/* Make Pop up menu "Add_Node" */
		if (ImGui::BeginPopup("Add_Node")) {
			auto newNodePos = (ImGui::GetMousePosOnOpeningCurrentPopup() - /*WindowPosition*/pos - m_Canvas.offset) / m_Canvas.zoom;

			for (uint32_t i = 1; i < m_NodeTypes.size(); i++) {
				if (ImGui::MenuItem(m_NodeTypes.at(i).c_str())) {
					AddNode(i, newNodePos);
				}
			}

			ImGui::EndPopup();
		}
		//m_Canvas.activeSlot = nullptr;


		return true;
	}

	void ShaderGraph::EndCanvas()
	{
		if (HasPendingConnection()) {
			RenderConnection(m_Canvas.activeSlot->pos, ImGui::GetIO().MousePos);
		}

		if (m_ErrorState.errorNode || m_ErrorState.errorSlot) {
			auto drawList = ImGui::GetWindowDrawList();
			ImVec2 bbmin = m_ErrorState.errorNode->pos;
			ImVec2 bbmax = m_ErrorState.errorNode->pos + m_ErrorState.errorNode->size;
			drawList->AddRect(bbmin, bbmax,
				IM_COL32(224, 43, 43, 255),
				8.f, ImDrawCornerFlags_All, 2.4f);
			drawList->AddRectFilled(bbmin, bbmax,
				IM_COL32(224, 43, 43, 74),
				8.f, ImDrawCornerFlags_All);
			drawList->AddCircle(m_ErrorState.errorSlot->pos, CIRCLE_RADIUS + 1.f, IM_COL32(224, 43, 43, 255), 12, 1.6f);
			drawList->AddCircleFilled(m_ErrorState.errorSlot->pos, CIRCLE_RADIUS + 0.6f, IM_COL32(224, 43, 43, 184));
		}

		ImGui::PopID();

		ImGui::PopStyleVar(2);
		//ImGui::GetFont()->Scale = 1.f;
		ImGui::PopFont();
	}

	////////////////////////////////////////////////////////////////
	////////////////////        Node            ////////////////////
	////////////////////////////////////////////////////////////////
	bool ShaderGraph::DrawNode(Node& node, std::vector<Node*>::const_iterator idx)
	{
		static bool rename = false;

		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiIO& io = ImGui::GetIO();

		/* Push the ID */
		ImGui::PushID(&node);
		//auto pos = ImGui::GetCursorScreenPos() + ImVec2{ 10.f, 0.f };
		auto pos = node.pos;
		auto size = node.size * ImGui::GetFontSize();
		//ImVec2 size{ 360.f, 80.f };
		auto screenPos = ImGui::GetWindowPos() + pos * m_Canvas.zoom + m_Canvas.offset;
		{
			auto maxScreenPos = screenPos + size;
			auto maxWindowsize = ImGui::GetWindowPos() + ImGui::GetWindowSize();
			if (screenPos.x > maxWindowsize.x || screenPos.y > maxWindowsize.y
				|| maxScreenPos.x < ImGui::GetWindowPos().x || maxScreenPos.y < ImGui::GetWindowPos().y) {
				ImGui::PopID();
				return true;
			}
		}
		ImGui::SetCursorScreenPos(screenPos);

		/* Add node title bar */
		ImRect titleRect{ screenPos, ImVec2{ screenPos.x + size.x, screenPos.y + TITLE_SIZE_Y} };
		auto titleID = ImGui::GetID("title");
		ImGui::ItemSize(titleRect.GetSize());
		ImGui::ItemAdd(titleRect, titleID);
		bool hovered, held;
		ImGui::ButtonBehavior(titleRect, titleID, &hovered, &held);
		if (held) {
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				node.pos += io.MouseDelta / m_Canvas.zoom;
			}
		}

		drawList->ChannelsSplit(2);

		drawList->ChannelsSetCurrent(0); // Background
		/* Draw node title bar */
		//ImGui::CollapseButton(ImGui::GetID("##Collapse"), { titleRect.Max.x, titleRect.Min.y }, nullptr);
		drawList->AddRectFilled(titleRect.Min, titleRect.Max, IM_COL32(84, 118, 176, 255), 8.f * m_Canvas.zoom, ImDrawCornerFlags_Top);
		drawList->ChannelsSetCurrent(1); // Foreground
		/* Draw node title text */
		if (rename && m_Canvas.activeNode == &node) {
			ImGui::SetCursorScreenPos(titleRect.Min + ImVec2{ 20.f, 1.f });
			ImGui::SetNextItemWidth(180.f * m_Canvas.zoom);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(102, 102, 106, 220));
			node.name.reserve(64);
			/* This is being rendered behind the title bar and is this not clickable */
			rename = !ImGui::InputText("rename", &node.name[0u], 64, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopStyleColor();
		}
		else {
			auto textSize = ImGui::CalcTextSize(node.name.c_str());
			drawList->AddText((titleRect.Min + titleRect.Max) / 2.f - textSize / 2.f, IM_COL32(255, 255, 255, 255), node.name.c_str());
		}

		auto bodyPos = ImVec2{ titleRect.Min.x, titleRect.Max.y + style.ItemSpacing.y };


		/* Draw foreground widgets of the node */
		drawList->ChannelsSetCurrent(1); // Foreground
		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(78, 85, 97, 185));
		ImGui::BeginGroup();
		ImGui::SetCursorScreenPos(bodyPos);
		/* Draw Input ports */
		for (uint16_t i = 0; i < node.inputSlots.size(); i++) {
			std::string id = "input##" + (char)i;

			ImGui::PushID(id.c_str());
			ImGui::SetCursorScreenPos(ImVec2{ bodyPos.x, ImGui::GetCursorScreenPos().y });
			DrawSlot(*node.inputSlots.at(i));
			ImGui::PopID();
		}

		ImGui::SetCursorScreenPos(bodyPos);

		/* Draw Output ports */
		for (uint16_t i = 0; i < node.outputSlots.size(); i++) {
			std::string id = "output##" + (char)i;

			ImGui::PushID(id.c_str());
			ImGui::SetCursorScreenPos(ImVec2{ bodyPos.x, ImGui::GetCursorScreenPos().y });
			DrawSlot(*node.outputSlots.at(i));
			ImGui::PopID();
		}

		ImGui::EndGroup();
		ImGui::PopStyleColor();

		/* Find size of node */
		auto id = ImGui::GetID(&node);
		ImVec2 bodySize = ImVec2(size.x, ImGui::GetItemRectSize().y);// ImGui::GetItemRectSize() + ImVec2(8.f, 0.f) + ImVec2(8.f, 0.f);
		//size.x -= CIRCLE_RADIUS + ImGui::GetStyle().ItemSpacing.x;
		ImRect nodeRect{ bodyPos, bodyPos + bodySize };
		nodeRect.Min -= ImVec2(0.f, style.ItemSpacing.y);
		ImGui::ItemSize(nodeRect.GetSize());
		if (!ImGui::ItemAdd(nodeRect, id)) {
			drawList->ChannelsMerge();
			ImGui::PopID();
			return false;
		}

		drawList->ChannelsSetCurrent(0);
		/* Draw node body */
		drawList->AddRectFilled(nodeRect.Min, nodeRect.Max, IM_COL32(64, 64, 68, 200), 8.f * m_Canvas.zoom, ImDrawCornerFlags_Bot);

		drawList->ChannelsMerge();

		/* Add an item to represent complete node */
		nodeRect.Min = titleRect.Min;
		node.size = nodeRect.GetSize() / ImGui::GetFontSize();
		nodeRect.Min -= ImVec2(CIRCLE_RADIUS, CIRCLE_RADIUS);
		nodeRect.Max += ImVec2(CIRCLE_RADIUS, CIRCLE_RADIUS);

		ImGui::ItemAdd(nodeRect, ImGui::GetID(node.name.c_str()));

		if (ImGui::IsItemHovered()) {
			drawList->AddRect(nodeRect.Min + ImVec2(CIRCLE_RADIUS, CIRCLE_RADIUS),
				nodeRect.Max - ImVec2(CIRCLE_RADIUS, CIRCLE_RADIUS),
				IM_COL32(127, 184, 223, 245),
				8.f, ImDrawCornerFlags_All, 1.6f);
		}

		if (&node != m_Nodes[0] && ImGui::IsItemClicked(ImGuiMouseButton_Right)) { // Cannot delete Result node
			ImGui::OpenPopup("node-context");
		}

		if (ImGui::BeginPopup("node-context")) {
			if (ImGui::MenuItem("Delete")) {
				m_Nodes.erase(idx);
				delete& node;
			}
			if (ImGui::MenuItem("Rename")) {
				rename = true;
				m_Canvas.activeNode = &node;
			}
			ImGui::EndPopup();
		}

		ImGui::PopID(); // node
		//ImGui::Button("Btn1");

		return true;
	}

	////////////////////////////////////////////////////////////////
	////////////////////        Slot            ////////////////////
	////////////////////////////////////////////////////////////////

	static uint32_t _GetDataInputSize(DataType dataType);

	bool ShaderGraph::DrawSlot(Slot& slot)
	{
		bool isOutputType = slot.type == Slot::OUTPUT_SLOT;

		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		/* Position the Cursor */
		ImVec2 title_size = ImGui::CalcTextSize(slot.name.c_str());

		auto initPos = ImGui::GetCursorPos();

		//------------ NOTE --------------------------------------------------
		// TODO: Instead of using Group maybe try using an item 
		//       This will also allow setting the bounding box to custom size
		//--------------------------------------------------------------------
		if (isOutputType) {
			initPos += ImVec2(slot.parentNode->size.x * ImGui::GetFontSize(), 0.f);
			ImGui::SetCursorPosX(initPos.x - title_size.x - CIRCLE_RADIUS - style.ItemSpacing.x); // output
			ImGui::BeginGroup(); // ImVec2 min = ImGui::GetCursorScreenPos() - ImVec2(some_x, some_y);
		}
		else {
			ImGui::SetCursorPosX(initPos.x - CIRCLE_RADIUS); // input
			ImGui::BeginGroup(); // ImVec2 min = ImGui::GetCursorScreenPos() - ImVec2(some_x, some_y);
			ImGui::SetCursorPosX(initPos.x + CIRCLE_RADIUS + style.ItemSpacing.x); // input
		}

		/* Draw the name */
		ImGui::TextUnformatted(slot.name.c_str());
		ImGui::SetCursorPos(initPos);

		ImRect circleRect{
			ImGui::GetCursorScreenPos(),
			ImGui::GetCursorScreenPos() + ImVec2{ CIRCLE_RADIUS * 2.f, CIRCLE_RADIUS * 2.f }
		};
		float offset_y = title_size.y / 2.f - CIRCLE_RADIUS;
		circleRect.Min.y += offset_y;
		circleRect.Max.y += offset_y;

		float offset_x = CIRCLE_RADIUS; // Circle radius
		circleRect.Min.x -= offset_x;
		circleRect.Max.x -= offset_x;

		ImGui::ItemSize(circleRect.GetSize());
		ImGui::ItemAdd(circleRect, ImGui::GetID(slot.name.c_str()));

		ImGui::EndGroup();

		bool selected = ImGui::IsItemClicked();

		bool hovered = ImGui::IsItemHovered();
		drawList->AddCircleFilled(circleRect.GetCenter(), CIRCLE_RADIUS, hovered ? IM_COL32(111, 201, 252, 255) : IM_COL32(143, 143, 148, 200));
		if (hovered) {
			drawList->AddCircle(circleRect.GetCenter(), CIRCLE_RADIUS + 3.f, IM_COL32(111, 201, 252, 255));
			//ImGui::BeginTooltip();
			//ImGui::TextUnformatted(m_DataTypes[(m_DataTypes.size() - 1 < slot.dataType) ? 0 : slot.dataType].c_str());
			//ImGui::EndTooltip();
		}

		slot.pos = circleRect.GetCenter();

		/*auto id = slot.name + "##gap";
		ImGui::PushID(id.c_str());
		ImRect slot_rect{ ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
		ImGui::ItemSize(slot_rect.GetSize());
		ImGui::ItemAdd(slot_rect, ImGui::GetID(id.c_str()));
		ImGui::PopID();*/

		if (slot.type == Slot::INPUT_SLOT) {
			if (!slot.connectedSlot) {
				auto inputSize = _GetDataInputSize(slot.dataType);
				auto center = circleRect.GetCenter();
				auto max = center + ImVec2{ -10.f, 14.f } *m_Canvas.zoom;
				auto min = max - ImVec2{ 10.f + (60.f + style.ItemInnerSpacing.x) * inputSize - style.ItemInnerSpacing.x, 28.f } *m_Canvas.zoom;
				drawList->AddRectFilled(min, max, IM_COL32(84, 84, 88, 220), 1.2f);
				//ImGui::PushID("##default");
				//ImGui::ItemAdd({ center - ImVec2{ 35.f, 14.f }, center + ImVec2{ 35.f, 14.f } }, ImGui::GetID((slot.name + "-default").c_str()));
				ImGui::SetCursorScreenPos({ min + ImVec2{ 5.f, 14.f - 9.5f } *m_Canvas.zoom });
				ImGui::SetNextItemWidth(60.f * inputSize * m_Canvas.zoom);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(102, 102, 106, 220));
				//ImGui::InputDouble("", &slot.defaultValue, 0.0, 0.0, "%.3f");
				DrawSlotInputBox(slot);
				ImGui::PopStyleColor();
			}
			else {
				RenderConnection(slot.pos, slot.connectedSlot->pos);
			}
		}
		

		if (selected) {
			if (!HasPendingConnection()) { // Does not have pending connection i.e. make new connection
				m_Canvas.activeSlot = &slot;
			}
			else { // Has a pending connection so finish that connection
				// 4. [TODO] Check that the connection does not cause cyclic dependencies (infinite loops)
				// 5. Save the link to OUTPUT_SLOT in the INPUT_SLOT
				if (m_Canvas.activeSlot->type != slot.type  // 1. Check type (input/output) -> Have to be different
					&& m_Canvas.activeSlot->parentNode != slot.parentNode  // 2. Check node -> Have to be different
					&& m_Canvas.activeSlot->dataType == slot.dataType) {  // 3. Check data type (vec1, vec2, ...) -> Have to be conforming
					if (slot.type == Slot::OUTPUT_SLOT) { // If the slot is OUTPUT then it is the INPUT for the connection
						m_Canvas.activeSlot->connectedSlot = &slot;
					}
					else {
						slot.connectedSlot = m_Canvas.activeSlot;
					}
				}

				/* End connection for any result */
				m_Canvas.activeSlot = nullptr;
			}
		}

		return selected;
	}

	void ShaderGraph::DrawSlotInputBox(Slot& slot)
	{
		switch (slot.dataType)
		{
		case Apex::EditorTools::BOOL:
			break;
		case Apex::EditorTools::INT:
			ImGui::InputInt("", &((SlotInt*)&slot)->value);
			break;
		case Apex::EditorTools::FLOAT:
			ImGui::InputFloat("", &((SlotFloat*)&slot)->value);
			break;
		case Apex::EditorTools::IVEC2:
			break;
		case Apex::EditorTools::IVEC3:
			break;
		case Apex::EditorTools::IVEC4:
			break;
		case Apex::EditorTools::VEC2:
			ImGui::InputFloat2("", &((SlotVec2*)&slot)->value[0]);
			break;
		case Apex::EditorTools::VEC3:
			ImGui::InputFloat3("", &((SlotVec3*)&slot)->value[0]);
			break;
		case Apex::EditorTools::VEC4:
			ImGui::InputFloat4("", &((SlotVec4*)&slot)->value[0]);
			break;
		case Apex::EditorTools::MAT2:
			break;
		case Apex::EditorTools::MAT3:
			break;
		case Apex::EditorTools::MAT4:
			break;
		case Apex::EditorTools::TEX2D:
			break;
		case Apex::EditorTools::TEX3D:
			break;
		default:
			break;
		}
	}

	////////////////////////////////////////////////////////////////
	//////////////////       Connection         ////////////////////
	////////////////////////////////////////////////////////////////

	bool ShaderGraph::RenderConnection(const ImVec2& input_pos, const ImVec2& output_pos, float thickness)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImGuiStyle& style = ImGui::GetStyle();

		thickness *= m_Canvas.zoom;

		float length = input_pos.x - output_pos.x;

		ImVec2 p2 = input_pos - ImVec2{ length /** m_Canvas.zoom*/, 0.f };
		ImVec2 p3 = output_pos + ImVec2{ length /** m_Canvas.zoom*/, 0.f };

#define SQR(_x_) _x_ * _x_ 

		ImVec2 closest_pt = ImBezierClosestPointCasteljau(input_pos, p2, p3, output_pos, ImGui::GetMousePos(), style.CurveTessellationTol);
		float min_square_distance = ImFabs(ImLengthSqr(ImGui::GetMousePos() - closest_pt));
		bool is_close = min_square_distance <= SQR(thickness + 0.8);
		draw_list->AddBezierCurve(input_pos, p2, p3, output_pos, is_close ? IM_COL32(111, 252, 151, 200) : IM_COL32(204, 56, 207, 250), is_close ? thickness + 0.5f : thickness, 0);
		return is_close;
	}

	uint32_t _GetDataInputSize(DataType dataType)
	{
		switch (dataType)
		{
		case Apex::EditorTools::BOOL:
		case Apex::EditorTools::INT:
		case Apex::EditorTools::FLOAT:
			return 1;
		case Apex::EditorTools::IVEC2:
		case Apex::EditorTools::VEC2:
			return 2;
		case Apex::EditorTools::IVEC3:
		case Apex::EditorTools::VEC3:
			return 3;
		case Apex::EditorTools::IVEC4:
		case Apex::EditorTools::VEC4:
			return 4;
		case Apex::EditorTools::MAT2:
		case Apex::EditorTools::MAT3:
		case Apex::EditorTools::MAT4:
		case Apex::EditorTools::TEX2D:
		case Apex::EditorTools::TEX3D:
		default:
			return 1;
		}
	}

}