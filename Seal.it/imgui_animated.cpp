#define IMGUI_DEFINE_MATH_OPERATORS

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"
#include "imgui_animated.h"
struct checkbox_element {
    float selected_rect;
};
bool ImGui::Toggle( const char* label, bool* v )
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = 16;
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

    window->DrawList->AddRectFilled(total_bb.Min, total_bb.Min + ImVec2(square_sz, square_sz), ImColor(32, 33, 35), 3.0f);
    window->DrawList->AddRectFilled(total_bb.Min, total_bb.Min + ImVec2(square_sz, square_sz), ImColor(22 / 255.0f, 118 / 255.0f, 243 / 255.0f, it_anim->second.selected_rect), 3.0f);
    RenderCheckMark(window->DrawList, ImVec2(total_bb.Min.x + 4, (total_bb.Min.y + total_bb.Max.y) / 2 - 4), ImColor(1.0f, 1.0f, 1.0f, it_anim->second.selected_rect), 9.0f);

    window->DrawList->AddText(ImVec2(total_bb.Max.x - label_size.x - 5, total_bb.Min.y), ImGui::GetColorU32(ImGuiCol_Text), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool ImGui::ButtonScrollableEx( const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags )
{
	ImGuiWindow* window = GetCurrentWindow( );
	if ( window->SkipItems )
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID( label );
	const ImVec2 label_size = CalcTextSize( label, NULL, true );

	ImVec2 pos = window->DC.CursorPos;
	if ( ( flags & ImGuiButtonFlags_AlignTextBaseLine ) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset ) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize( size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f );

	const ImRect bb( pos, pos + size );
	ItemSize( size, style.FramePadding.y );
	if ( !ItemAdd( bb, id ) )
		return false;


	bool hovered, held;
	bool pressed = ButtonBehavior( bb, id, &hovered, &held, flags );

	const ImU32 col = GetColorU32( ( held && hovered ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button );
	RenderNavHighlight( bb, id );
	RenderFrame( bb.Min, bb.Max, col, true, style.FrameRounding );

	const float offset = size.x >= label_size.x + style.FramePadding.x * 2.0f ? size.x + style.FramePadding.x : static_cast< int >( g.Time * 60.f ) % static_cast< int >( label_size.x + size.x + style.FramePadding.x * 2.f + 4 );
	const ImRect text = ImRect( ImVec2( bb.Min.x + size.x - offset + style.FramePadding.x * 2.f, bb.Min.y + style.FramePadding.y ), bb.Max - style.FramePadding );

	RenderTextClipped( text.Min, text.Max, label, NULL, &label_size, size.x >= label_size.x + style.FramePadding.x * 2.0f ? g.Style.ButtonTextAlign : ImVec2( 0, 0 ), &bb );
	return pressed;
}

bool ImGui::ButtonScrollable( const char* label, const ImVec2& size_arg )
{
	return ButtonScrollableEx( label, size_arg, ImGuiButtonFlags_None );
}