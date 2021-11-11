#include "apexed_pch.h"
#include "ImGuiCustom.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>

namespace Apex {

	static ImU32 GetTint(const ImU32& col, float factor)
	{
		ImU8 col_r = (col >> IM_COL32_R_SHIFT) & 0xFF;
		ImU8 col_g = (col >> IM_COL32_G_SHIFT) & 0xFF;
		ImU8 col_b = (col >> IM_COL32_B_SHIFT) & 0xFF;
		ImU8 col_a = (col >> IM_COL32_A_SHIFT) & 0xFF;

		col_r = col_r + (ImU8)((255 - col_r) * factor);
		col_g = col_g + (ImU8)((255 - col_g) * factor);
		col_b = col_b + (ImU8)((255 - col_b) * factor);

		ImU32 tint = (ImU32)col_r << IM_COL32_R_SHIFT;
		tint |= (ImU32)col_g << IM_COL32_G_SHIFT;
		tint |= (ImU32)col_b << IM_COL32_B_SHIFT;
		tint |= (ImU32)col_a << IM_COL32_A_SHIFT;
		return tint;
	}

	static ImU32 GetShade(const ImU32& col, float factor)
	{
		float shadeFactor = 1.f - factor;

		ImU8 col_r = (col >> IM_COL32_R_SHIFT) & 0xFF;
		ImU8 col_g = (col >> IM_COL32_G_SHIFT) & 0xFF;
		ImU8 col_b = (col >> IM_COL32_B_SHIFT) & 0xFF;
		ImU8 col_a = (col >> IM_COL32_A_SHIFT) & 0xFF;

		col_r = (ImU8)(col_r * shadeFactor);
		col_g = (ImU8)(col_g * shadeFactor);
		col_b = (ImU8)(col_b * shadeFactor);

		ImU32 shade = (ImU32)col_r << IM_COL32_R_SHIFT;
		shade |= (ImU32)col_g << IM_COL32_G_SHIFT;
		shade |= (ImU32)col_b << IM_COL32_B_SHIFT;
		shade |= (ImU32)col_a << IM_COL32_A_SHIFT;
		return shade;
	}

	void internal::RenderFrameGradient(ImVec2 p_min, ImVec2 p_max, ImU32 upper_col, ImU32 lower_col, bool border, float rounding)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
	    window->DrawList->AddRectFilledMultiColor(p_min, p_max, upper_col, upper_col, lower_col, lower_col);
	    const float border_size = 1.5f/*g.Style.FrameBorderSize*/;
	    if (border && border_size > 0.0f)
	    {
	        window->DrawList->AddRect(p_min + ImVec2(0.5f, 0.5f), p_max + ImVec2(0.5f, 0.5f), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, 0, border_size);
	        window->DrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, 0, border_size);
	    }
	}

	bool internal::ButtonGradientEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
	{
	    ImGuiWindow* window = ImGui::GetCurrentWindow();
	    if (window->SkipItems)
	        return false;

	    ImGuiContext& g = *GImGui;
	    const ImGuiStyle& style = g.Style;
	    const ImGuiID id = window->GetID(label);
	    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	    ImVec2 pos = window->DC.CursorPos;
	    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
	        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	    const ImRect bb(pos, pos + size);
	    ImGui::ItemSize(size, style.FramePadding.y);
	    if (!ImGui::ItemAdd(bb, id))
	        return false;

	    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
	        flags |= ImGuiButtonFlags_Repeat;

	    bool hovered, held;
	    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	    // Render
		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		const ImU32 col_tint = GetTint(col, 0.2f);
		const ImU32 col_shade = GetShade(col, 0.2f);
	    ImGui::RenderNavHighlight(bb, id);
	    RenderFrameGradient(bb.Min, bb.Max, col_tint, col_shade, true, 2.5f/*style.FrameRounding*/);

	    if (g.LogEnabled)
		    ImGui::LogSetNextTextDecoration("[", "]");
	    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	    // Automatically close popups
	    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	    //    CloseCurrentPopup();

	    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	    return pressed;
	}
	
	bool Button(const char* label, const ImVec2& size_arg)
	{
		return internal::ButtonGradientEx(label, size_arg, ImGuiButtonFlags_None);
	}

	void SetImGuiStyle(ImGuiStyle* dst)
	{
		ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
		//style->FramePadding = 

	    ImVec4* colors = style->Colors;

	    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	    colors[ImGuiCol_WindowBg]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	    colors[ImGuiCol_Border]                 = ImVec4(0.09f, 0.09f, 0.10f, 0.94f);
	    colors[ImGuiCol_BorderShadow]           = ImVec4(0.43f, 0.43f, 0.43f, 0.72f);
	    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	    colors[ImGuiCol_TitleBg]                = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
	    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
	    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
	    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
	    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
	    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
	    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
	    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

}
