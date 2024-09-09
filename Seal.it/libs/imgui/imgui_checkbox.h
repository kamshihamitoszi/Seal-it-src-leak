#pragma once

#include "../../imgui/imgui.h"

namespace ImGui
{
	IMGUI_API bool Toggle(const char* label, bool* v);
	IMGUI_API bool AddRectMultiColorRounded(const ImVec2& p_min, const ImVec2& p_max, float rounding, ImU32 col_tl, ImU32 col_tr, ImU32 col_br, ImU32 col_bl);
	IMGUI_API bool ButtonScrollableEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
	IMGUI_API bool ButtonScrollable(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
}