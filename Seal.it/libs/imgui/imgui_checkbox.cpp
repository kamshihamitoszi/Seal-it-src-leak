#define IMGUI_DEFINE_MATH_OPERATORS

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "imgui_checkbox.h"
#include <map>
#include <cmath>

void AddRectMultiColorRounded(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float rounding, ImU32 col_tl, ImU32 col_tr, ImU32 col_br, ImU32 col_bl)
{
	// Sprawdzenie, czy zaokr¹glenie jest wiêksze ni¿ po³owa szerokoœci/wysokoœci prostok¹ta
	rounding = ImMin(rounding, ImFloor((p_max.x - p_min.x) * 0.5f));
	rounding = ImMin(rounding, ImFloor((p_max.y - p_min.y) * 0.5f));

	// Rysowanie prostok¹ta z zaokr¹glonymi rogami
	draw_list->AddQuadFilled(p_min, ImVec2(p_max.x, p_min.y), p_max, ImVec2(p_min.x, p_max.y), col_tl);

	// Rysowanie segmentów prostok¹ta
	draw_list->AddTriangleFilled(p_min, ImVec2(p_min.x + rounding, p_min.y), ImVec2(p_min.x, p_min.y + rounding), col_tl);
	draw_list->AddTriangleFilled(ImVec2(p_max.x - rounding, p_min.y), ImVec2(p_max.x, p_min.y), ImVec2(p_max.x, p_min.y + rounding), col_tr);
	draw_list->AddTriangleFilled(ImVec2(p_max.x, p_max.y - rounding), ImVec2(p_max.x, p_max.y), ImVec2(p_max.x - rounding, p_max.y), col_br);
	draw_list->AddTriangleFilled(ImVec2(p_min.x, p_max.y), ImVec2(p_min.x + rounding, p_max.y), ImVec2(p_min.x, p_max.y - rounding), col_bl);

	// Rysowanie zaokr¹glonych rogów
	const int segments = 12;
	const float theta = IM_PI * 0.5f / segments;
	const float c = cosf(theta);
	const float s = sinf(theta);
	const float t = 1.0f - c;

	ImVec2 p[4];
	ImVec2 v[4];
	for (int i = 0; i < 4; i++)
	{
		v[i] = ImVec2((i == 1 || i == 2) ? (i == 2 ? p_max.x : p_min.x) : (i == 1 ? p_max.x : p_min.x),
			(i == 0 || i == 3) ? (i == 3 ? p_max.y : p_min.y) : (i == 0 ? p_max.y : p_min.y));

		if (i == 0)
			p[i] = ImVec2(v[i].x + rounding * t, v[i].y - rounding * s);
		else if (i == 1)
			p[i] = ImVec2(v[i].x - rounding * s, v[i].y - rounding * t);
		else if (i == 2)
			p[i] = ImVec2(v[i].x - rounding * t, v[i].y + rounding * s);
		else
			p[i] = ImVec2(v[i].x + rounding * s, v[i].y + rounding * t);

		draw_list->AddQuadFilled(p[i], v[i], ImVec2(v[i].x, v[i].y), ImVec2(v[i].x, v[i].y), col_tl);
	}
}
struct checkbox_element {
	float selected_rect;
};
bool ImGui::Toggle(const char* label, bool* v)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	const float square_sz = 15;
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + ImVec2(square_sz + 12 + label_size.x, (label_size.y > square_sz) ? label_size.y : square_sz));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*v = !(*v);
		MarkItemEdited(id);
	}

	static std::map <ImGuiID, checkbox_element> anim;
	auto it_anim = anim.find(id);
	if (it_anim == anim.end())
	{
		anim.insert({ id, { 0.0f } });
		it_anim = anim.find(id);
	}

	it_anim->second.selected_rect = ImLerp(it_anim->second.selected_rect, *v ? 1.0f : (hovered ? 0.2f : 0.0f), 0.08f * (1.0f - ImGui::GetIO().DeltaTime));

	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Min + ImVec2(square_sz, square_sz), ImColor(32, 33, 35), 50.0f);
	window->DrawList->AddRectFilled(total_bb.Min, total_bb.Min + ImVec2(square_sz, square_sz), ImColor(255 / 255.0f, 30 / 255.0f, 70 / 255.0f, it_anim->second.selected_rect), 50.0f);

	window->DrawList->AddText(ImVec2(total_bb.Max.x - label_size.x - 5, total_bb.Min.y -2 ), ImGui::GetColorU32(ImGuiCol_Text), label);

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;





	
}

bool ImGui::ButtonScrollableEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;


	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	const float offset = size.x >= label_size.x + style.FramePadding.x * 2.0f ? size.x + style.FramePadding.x : static_cast<int>(g.Time * 60.f) % static_cast<int>(label_size.x + size.x + style.FramePadding.x * 2.f + 4);
	const ImRect text = ImRect(ImVec2(bb.Min.x + size.x - offset + style.FramePadding.x * 2.f, bb.Min.y + style.FramePadding.y), bb.Max - style.FramePadding);

	RenderTextClipped(text.Min, text.Max, label, NULL, &label_size, size.x >= label_size.x + style.FramePadding.x * 2.0f ? g.Style.ButtonTextAlign : ImVec2(0, 0), &bb);
	return pressed;
}

bool ImGui::ButtonScrollable(const char* label, const ImVec2& size_arg)
{
	return ButtonScrollableEx(label, size_arg, ImGuiButtonFlags_None);
}