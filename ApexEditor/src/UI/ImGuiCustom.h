#pragma once

#include <imgui.h>

namespace Apex {

	void SetImGuiStyle(ImGuiStyle* dst = nullptr);

	namespace internal {
		void RenderFrameGradient(ImVec2 p_min, ImVec2 p_max, ImU32 upper_col, ImU32 lower_col, bool border = true, float rounding = 0.0f);
		bool ButtonGradientEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
	}

	bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));

}