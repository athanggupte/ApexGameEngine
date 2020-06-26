#include <apex_pch.h>
#include "Node.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <imgui_internal.h>

namespace Apex {

#define NODE_SIZE_X 220.f
#define TITLE_SIZE_Y 18.f
#define CIRCLE_RADIUS 4.9f

	////////////////////////////////////////////////////////////////
	////////////////////       Canvas           ////////////////////
	////////////////////////////////////////////////////////////////

	bool BeginCanvas(NodeCanvas& canvas)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImGuiWindow* w = ImGui::GetCurrentWindow();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushID(&canvas);
		if (!ImGui::ItemAdd(w->ContentRegionRect, ImGui::GetID("canvas"))) {
			ImGui::PopID();
			return false;
		}


		/* Get Mouse Interactions */
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
				canvas.offset += io.MouseDelta;

			if (io.MouseWheel != 0) {
				ImVec2 mouseRel = ImVec2{ ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y };
				float prevZoom = canvas.zoom;
				canvas.zoom = ImClamp(canvas.zoom + io.MouseWheel * canvas.zoom / 16.f, 0.3f, 1.5f);
				float zoomFactor = (prevZoom - canvas.zoom) / prevZoom;
				canvas.offset += (mouseRel - canvas.offset) * zoomFactor;
			}
		}

		/* Draw Grid */
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		//const ImVec2 windowMin = w->ContentRegionRect.Min;
		//const ImVec2 windowMax = w->ContentRegionRect.Max;
		const float gridSize = 64.f * canvas.zoom;
		//auto gridColor = IM_COL32(150, 150, 150, 150);
		auto gridColor = ImColor(style.Colors[ImGuiCol_Separator]);

		for (float x = fmodf(canvas.offset.x, gridSize); x < size.x; x += gridSize) {
			//drawList->AddLine(ImVec2(x, windowMin.y), ImVec2(x, windowMax.y), gridColor);
			drawList->AddLine(ImVec2(x, 0) + pos, ImVec2(x, size.y) + pos, gridColor, 2.2f);
		}
		for (float y = fmodf(canvas.offset.y, gridSize); y < size.y; y += gridSize) {
			//drawList->AddLine(ImVec2(windowMin.x, y), ImVec2(windowMax.x, y), gridColor);
			drawList->AddLine(ImVec2(0, y) + pos, ImVec2(size.x, y) + pos, gridColor, 2.2f);
		}

		//ImGui::SetWindowFontScale(canvas.zoom);

		ImGui::PopID();

		canvas.activeSlot = nullptr;

		return true;
	}

	void EndCanvas(NodeCanvas& canvas)
	{
		if (canvas.activeSlot) {
			canvas.connections.push_back({ canvas.activeSlot->parentNode, canvas.activeSlot });
		}

		for (auto& connection : canvas.connections) {
			DrawConnection(connection, canvas);
		}
	}

	////////////////////////////////////////////////////////////////
	////////////////////        Node            ////////////////////
	////////////////////////////////////////////////////////////////
	bool DrawNode(Node& node, NodeCanvas& canvas)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		
		/* Push the ID */
		ImGui::PushID(&node);
		//auto pos = ImGui::GetCursorScreenPos() + ImVec2{ 10.f, 0.f };
		auto pos = node.pos;
		//ImVec2 size{ 360.f, 80.f };
		auto screenPos = ImGui::GetWindowPos() + pos * canvas.zoom + canvas.offset;
		{
			auto maxScreenPos = screenPos + node.size;
			auto maxWindowsize = ImGui::GetWindowPos() + ImGui::GetWindowSize();
			if (screenPos.x > maxWindowsize.x || screenPos.y > maxWindowsize.y 
				|| maxScreenPos.x < ImGui::GetWindowPos().x || maxScreenPos.y < ImGui::GetWindowPos().y) {
				ImGui::PopID();
				return true;
			}
		}
		ImGui::SetCursorScreenPos(screenPos);

		/* Add node title bar */
		ImRect titleRect{ screenPos, ImVec2{ screenPos.x + NODE_SIZE_X, screenPos.y + TITLE_SIZE_Y} };
		auto titleID = ImGui::GetID("title");
		ImGui::ItemSize(titleRect.GetSize());
		ImGui::ItemAdd(titleRect, titleID);
		ImGui::ButtonBehavior(titleRect, titleID, nullptr, nullptr);

		drawList->ChannelsSetCurrent(0); // Background
		/* Draw node title bar */
		drawList->AddRectFilled(titleRect.Min, titleRect.Max, IM_COL32(84, 118, 176, 255), 8.f, ImDrawCornerFlags_Top);
		//ImGui::CollapseButton(ImGui::GetID("##Collapse"), nodeRect.Min + ImVec2(8.f, 0.f), nullptr);
		/* Draw node title text */
		auto textSize = ImGui::CalcTextSize(node.name.c_str());
		drawList->AddText((titleRect.Min + titleRect.Max) / 2.f - textSize / 2.f, IM_COL32(255, 255, 255, 255), node.name.c_str());

		auto bodyPos = ImVec2{ titleRect.Min.x, titleRect.Max.y + style.ItemSpacing.y };

		drawList->ChannelsSplit(2);
		
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
			DrawSlot(node.inputSlots.at(i), canvas);
			ImGui::PopID();
		}

		ImGui::SetCursorScreenPos(bodyPos);

		/* Draw Output ports */
		for (uint16_t i = 0; i < node.outputSlots.size(); i++) {
			std::string id = "output##" + (char)i;

			ImGui::PushID(id.c_str());
			ImGui::SetCursorScreenPos(ImVec2{ bodyPos.x, ImGui::GetCursorScreenPos().y });
			DrawSlot(node.outputSlots.at(i), canvas);
			ImGui::PopID();
		}

		ImGui::EndGroup();
		ImGui::PopStyleColor();

		/* Find size of node */
		auto id = ImGui::GetID(&node);
		ImVec2 size = ImVec2(NODE_SIZE_X, ImGui::GetItemRectSize().y);// ImGui::GetItemRectSize() + ImVec2(8.f, 0.f) + ImVec2(8.f, 0.f);
		//size.x -= CIRCLE_RADIUS + ImGui::GetStyle().ItemSpacing.x;
		ImRect nodeRect{ bodyPos, bodyPos + size };
		ImGui::ItemSize(nodeRect.GetSize());
		if (!ImGui::ItemAdd(nodeRect, id)) {
			drawList->ChannelsMerge();
			ImGui::PopID();
			return false;
		}

		drawList->ChannelsSetCurrent(0);
		/* Draw node body */
		drawList->AddRectFilled(nodeRect.Min - ImVec2(0.f, style.ItemSpacing.y), nodeRect.Max, IM_COL32(64, 64, 68, 200), 8.f, ImDrawCornerFlags_Bot);

		drawList->ChannelsMerge();

		node.size = nodeRect.GetSize();

		ImGui::PopID(); // node

		//ImGui::Button("Btn1");

		return true;
	}

	////////////////////////////////////////////////////////////////
	////////////////////        Slot            ////////////////////
	////////////////////////////////////////////////////////////////

	bool DrawSlot(Slot& slot, NodeCanvas& canvas)
	{
		bool isOutputType = slot.type == Slot::OUTPUT_SLOT;

		const ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		/* Position the Cursor */
		ImVec2 title_size = ImGui::CalcTextSize(slot.name.c_str());

		auto initPos = ImGui::GetCursorPos() + ((isOutputType) ? ImVec2(NODE_SIZE_X, 0.f) : ImVec2(0.f, 0.f));
		
		//------------ NOTE --------------------------------------------------
		// TODO: Instead of using Group maybe try using an item 
		//       This will also allow setting the bounding box to custom size
		//--------------------------------------------------------------------
		if (isOutputType) {
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

		//ImGui::SameLine();
		//ImGui::SetCursorPosX(ImGui::GetCursorPosX() - CIRCLE_RADIUS);
		//ImGui::Text("Input #%d", i);
		ImGui::EndGroup();

		bool selected = ImGui::IsItemClicked();

		bool hovered = ImGui::IsItemHovered();
		drawList->AddCircleFilled(circleRect.GetCenter(), CIRCLE_RADIUS, hovered ? IM_COL32(111, 201, 252, 255) : IM_COL32(143, 143, 148, 200));
		if (hovered) {
			drawList->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(245, 206, 66, 255));
			drawList->AddCircle(circleRect.GetCenter(), CIRCLE_RADIUS + 3.f, IM_COL32(111, 201, 252, 255));
		}


		auto id = slot.name + "##gap";
		ImGui::PushID(id.c_str());
		ImRect slot_rect{ ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
		ImGui::ItemSize(slot_rect.GetSize());
		ImGui::ItemAdd(slot_rect, ImGui::GetID(id.c_str()));
		ImGui::PopID();

		if (selected) {
			canvas.activeSlot = &slot;
			canvas.activeSlotPos = circleRect.GetCenter();
		}

		return selected;
	}

	////////////////////////////////////////////////////////////////
	//////////////////       Connection         ////////////////////
	////////////////////////////////////////////////////////////////

	bool RenderConnection(const ImVec2& input_pos, const ImVec2& output_pos, float thickness, NodeCanvas& canvas)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImGuiStyle& style = ImGui::GetStyle();

		thickness *= canvas.zoom;

		ImVec2 p2 = input_pos - ImVec2{ 100 * canvas.zoom, 0 };
		ImVec2 p3 = output_pos + ImVec2{ 100 * canvas.zoom, 0 };

		ImVec2 closest_pt = ImBezierClosestPointCasteljau(input_pos, p2, p3, output_pos, ImGui::GetMousePos(), style.CurveTessellationTol);
		float min_square_distance = ImFabs(ImLengthSqr(ImGui::GetMousePos() - closest_pt));
		bool is_close = min_square_distance <= thickness * thickness;
		draw_list->AddBezierCurve(input_pos, p2, p3, output_pos, is_close ? IM_COL32(111, 252, 151, 200) : IM_COL32(204, 56, 207, 250), thickness, 0);
		return is_close;
	}

	bool DrawConnection(Connection& connection, NodeCanvas& canvas)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (connection.IsPending()) {
			RenderConnection(canvas.activeSlotPos, io.MousePos, 1.2f, canvas);
		}
		else {


		}

		return true;
	}
}