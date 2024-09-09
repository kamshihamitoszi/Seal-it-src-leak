#include "gui.h"
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../../libs/imgui/imgui.h"
#include "../../../libs/imgui/imgui_impl_opengl3.h"
#include "../../../libs/imgui/imgui_impl_glfw.h"
#include "../../../libs/imgui/imgui_impl_win32.h"
#include "../../settings/settings.h"

#include "imguipp.h"
#include "fonts.h"

#include "../array_list/array_list.h"
#include "../../imgui_animated.h"
#include "../hooks/hooks.h"


HWND hwnd = nullptr;
Gui* Gui::Get() {
	static Gui* gui = new Gui();
	return gui;
}
WNDPROC hGameWindowProc;
bool menuShown = false;
WNDPROC oWndProc;


extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (menuShown) {
		CallWindowProc(ImGui_ImplWin32_WndProcHandler, hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void SetWindowProc(bool state) {
	if (state) {
		oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
	}
	else {
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	}
}

LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (menuShown) {
		CallWindowProc(ImGui_ImplWin32_WndProcHandler, hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(hGameWindowProc, hWnd, uMsg, wParam, lParam);
}

void Gui::InitFonts() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	font2 = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 14);
	font1 = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 16.5f);

	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 2.5;
	icons_config.OversampleV = 2.5;

	font1 = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 20, &icons_config, icons_ranges);
}


inline float rgb_color[3] = { 54.f / 255, 54.f / 255, 54.f / 255 };




void Gui::init_style() {


	ImGuiStyle& Style = ImGui::GetStyle();

	Style.FramePadding = ImVec2(0, 0); //mniejsze ikony
	Style.ItemSpacing = ImVec2(5, 0);  //odstep


	Style.WindowRounding = 10.000f;
	Style.WindowTitleAlign = ImVec2(0.490f, 0.520f);
	Style.ChildRounding = 6.000f;
	Style.PopupRounding = 6.000f;
	Style.FrameRounding = 6.000f;
	Style.FrameBorderSize = 1.000f;
	Style.GrabRounding = 12.000f;
	Style.TabRounding = 7.000f;
	Style.ButtonTextAlign = ImVec2(0.510f, 0.490f);



	Style.Colors[ImGuiCol_Text] = ImVec4(173.f, 45.f, 247.f, 1.0f);
	Style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.440f, 0.440f, 0.440f, 1.000f);
	Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.012f, 0.012f, 0.012f, 1.000f);
	Style.Colors[ImGuiCol_PopupBg] = ImVec4(0.080f, 0.080f, 0.080f, 0.940f);
	Style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.000f);
	Style.Colors[ImGuiCol_FrameBg] = ImVec4(0.110f, 0.110f, 0.110f, 1.000f);
	Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(100.f, 108.f, 152.f, 1.0f);
	Style.Colors[ImGuiCol_TitleBg] = ImVec4(0.510f, 0.360f, 0.150f, 1.000f);
	Style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.910f, 0.640f, 0.130f, 1.000f);
	Style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.000f, 0.000f, 0.000f, 0.510f);
	Style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.110f, 0.110f, 0.110f, 1.000f);
	Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.060f, 0.060f, 0.060f, 0.530f);
	Style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.210f, 0.210f, 0.210f, 1.000f);
	Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.470f, 0.470f, 0.470f, 1.000f);
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.810f, 0.830f, 0.810f, 1.000f);
	Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.380f, 0.380f, 0.380f, 1.000f);
	Style.Colors[ImGuiCol_SliderGrab] = ImVec4(229.0f, 84.0f, 255.0f, 1.000f);
	Style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.910f, 0.640f, 0.130f, 1.000f);
	Style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.000f);
	Style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.380f, 0.380f, 0.380f, 1.000f);
	Style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.590f, 0.590f, 0.590f, 1.000f);
	Style.Colors[ImGuiCol_Header] = ImVec4(0.510f, 0.360f, 0.150f, 1.000f);
	Style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.910f, 0.640f, 0.130f, 1.000f);
	Style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.930f, 0.650f, 0.140f, 1.000f);
	Style.Colors[ImGuiCol_Separator] = ImVec4(0.210f, 0.210f, 0.210f, 1.000f);
	Style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.910f, 0.640f, 0.130f, 1.000f);
	Style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.780f, 0.550f, 0.210f, 1.000f);
	Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.210f, 0.210f, 0.210f, 1.000f);
	Style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.910f, 0.640f, 0.130f, 1.000f);
	Style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.780f, 0.550f, 0.210f, 1.000f);
	Style.Colors[ImGuiCol_Tab] = ImVec4(0.510f, 0.360f, 0.150f, 1.000f);
	Style.Colors[ImGuiCol_TabHovered] = ImVec4(0.910f, 0.640f, 0.130f, 1.000f);
	Style.Colors[ImGuiCol_TabActive] = ImVec4(105.0f, 101.0f, 100.0f, 1.000f);
	Style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.070f, 0.100f, 0.150f, 0.970f);
	Style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.140f, 0.260f, 0.420f, 1.000f);
	Style.Colors[ImGuiCol_PlotLines] = ImVec4(0.610f, 0.610f, 0.610f, 1.000f);
	Style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.430f, 0.350f, 1.000f);
	Style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.260f, 0.590f, 0.980f, 0.350f);
	Style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.260f, 0.590f, 0.980f, 1.000f);
	Style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800f, 0.800f, 0.800f, 0.350f);





	static bool fonts_inited;
	if (!fonts_inited) {
		fonts_inited = true;
		InitFonts();
	}

}



void Gui::ImGuiInit() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

	hGameWindowProc = (WNDPROC)SetWindowLongPtr(hwnd,
		GWLP_WNDPROC, (LONG_PTR)windowProc_hook);


	glewInit();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	ImGui::GetStyle().AntiAliasedFill = false;
	ImGui::GetStyle().AntiAliasedLines = false;
	ImGui::CaptureMouseFromApp();
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);

	this->init_style();
}

void Gui::ImGuiUnInit() {
	SetWindowProc(false);
}

float clip(float n, float lower, float upper)
{
	n = (n > lower) * n + !(n > lower) * lower;
	return (n < upper) * n + !(n < upper) * upper;
}

bool mWasOpened(static bool wasOpened) {

	if (menuShown) {
		wasOpened = true;
	}
	if (!(menuShown)) {
		wasOpened = false;
	}
	return wasOpened;
}

static int tab = 4;
ImFont* subtab_title;

static const char* items[]{ "Rainbow", "Color" };
static const unsigned int items_size = (sizeof(items) / sizeof(*items));
static int item = 0;
void AddFilledRectWithDifferentRoundings(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col,
	float rounding_tl, float rounding_tr, float rounding_br, float rounding_bl) {
	if ((col & IM_COL32_A_MASK) == 0)
		return;

	const int num_segments = 12; // Number of segments for rounded corners
	draw_list->PathClear();

	// Top-left corner
	if (rounding_tl > 0.0f) {
		draw_list->PathArcTo(ImVec2(p_min.x + rounding_tl, p_min.y + rounding_tl), rounding_tl, IM_PI, IM_PI * 1.5f, num_segments);
	}
	else {
		draw_list->PathLineTo(p_min);
	}

	// Top-right corner
	if (rounding_tr > 0.0f) {
		draw_list->PathArcTo(ImVec2(p_max.x - rounding_tr, p_min.y + rounding_tr), rounding_tr, IM_PI * 1.5f, IM_PI * 2.0f, num_segments);
	}
	else {
		draw_list->PathLineTo(ImVec2(p_max.x, p_min.y));
	}

	// Bottom-right corner
	if (rounding_br > 0.0f) {
		draw_list->PathArcTo(ImVec2(p_max.x - rounding_br, p_max.y - rounding_br), rounding_br, 0.0f, IM_PI * 0.5f, num_segments);
	}
	else {
		draw_list->PathLineTo(ImVec2(p_max.x, p_max.y));
	}

	// Bottom-left corner
	if (rounding_bl > 0.0f) {
		draw_list->PathArcTo(ImVec2(p_min.x + rounding_bl, p_max.y - rounding_bl), rounding_bl, IM_PI * 0.5f, IM_PI, num_segments);
	}
	else {
		draw_list->PathLineTo(ImVec2(p_min.x, p_max.y));
	}

	draw_list->PathFillConvex(col);
}
void Gui::Render() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_INSERT) & 1) {

		ImGuiIO& io = ImGui::GetIO();
		menuShown = !menuShown;

		if (menuShown) {
			io.ConfigFlags -= ImGuiConfigFlags_NoMouseCursorChange;
		}
		else {
			io.ConfigFlags -= ImGuiConfigFlags_NoMouseCursorChange;
		}
	}



	static float child_x_size = 154;

	if (menuShown) {
		ImGui::PushFont(font1);

		ImGui::SetNextWindowSize(ImVec2(873, 467));
		ImGui::SetColorEditOptions(ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
		ImGui::Begin("", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings );






		ImGui::PushStyleColor(ImGuiCol_ChildBg | ImGuiCol_Border, ImColor(8, 8, 8).Value);
		ImGui::BeginChild("LeftSide", ImVec2(child_x_size , ImGui::GetWindowSize().x));
		{

			ImGui::Dummy({ 25, 10 });
			
			const char* tabs[] = {
		 ICON_FA_CROSSHAIRS " combat",
		 ICON_FA_EYE " visuals",
		 ICON_FA_BOLT " misc",
		 
		 ICON_FA_WRENCH " unload",



			};

			ImGui::BeginGroup();
			for (int i = 0; i < ARRAYSIZE(tabs); i++)
			{
				if (ImGui::Button(tabs[i], ImVec2(141, 45)))
					tab = i;
				ImGui::Dummy({ 0, 10 });
			}

			

		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::SameLine();





		

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15);
		ImGui::BeginChild("RightSide", ImVec2(1000, ImGui::GetContentRegionAvail().y), false);
		{

			ImGui::BeginGroup();
			if (tab == 0)
			{
				{
					ImGui::Dummy({ 2, 0.0f });
					ImGui::PushStyleColor(ImGuiCol_ChildBg | ImGuiCol_Border, ImColor(8, 8, 8).Value);
					ImGui::BeginChild("", ImVec2(300, 200), false);
					if (ImGui::Toggle("aimassist", &settings::combat->aimassist_show)) {
						g_array_list.toggle_module("    aimassist");
					}
					
					if (settings::combat->aimassist_show) {



						ImGui::Dummy({ 0, 2.0f }); //odstep

						ImGui::Text("Speed ");
						ImGui::SameLine();
						ImGui::SliderFloat("##Speed", &settings::combat->aimassist_speed, 0.01, 5);

						ImGui::Dummy({ 0, 15.0f });

						ImGui::Text("Range ");
						ImGui::SameLine();
						ImGui::SliderFloat("##Range", &settings::combat->aimassist_distance, 1, 10);

						ImGui::Dummy({ 0, 20.0f });


						ImGui::Text("Bind: ");
						ImGui::SameLine();
						ImGui::Bind("##Bind", &settings::binds->aimassistbind, ImVec2(60, 20));

					}
					ImGui::Dummy({ 0, 10.0f });
					ImGui::EndChild();
					ImGui::PopStyleColor();
				}


			}
			if (tab == 2)
			{
				{
					ImGui::PushStyleColor(ImGuiCol_ChildBg | ImGuiCol_Border, ImColor(8, 8, 8).Value);
					ImGui::BeginChild("", ImVec2(300, 95), false);
					if (ImGui::Toggle("timer", &settings::movement->speedhack_show)) {
						g_array_list.toggle_module("    timer");
						
					}
					if (settings::movement->speedhack_show) {

						ImGui::Dummy({ 0, 1.0f }); //odstep

						ImGui::Text("Value: ");
						ImGui::SameLine();
						ImGui::SliderFloat("##Value", &settings::movement->SpeedhackSpeed, 1, 3);

						ImGui::Dummy({ 0, 10.0f }); //odstep

						ImGui::Text("Bind: ");
						ImGui::SameLine();
						ImGui::Bind("##Speedhack Bind", &settings::binds->speedhackbind, ImVec2(60, 20));
					}
						ImGui::Dummy({ 0, 10.0f });
						if (ImGui::Toggle("blink", &settings::misc->blink_show)) {
							g_array_list.toggle_module("    blink");
						}

						if (settings::misc->blink_show) {
							ImGui::Dummy({ 0, 5.0f });
							ImGui::Text("Bind: ");
							ImGui::SameLine();
							ImGui::Bind("##Blink bind", &settings::binds->blinkbind, ImVec2(60, 20));

							ImGui::Dummy({ 0, 2.0f });
						}


					
					ImGui::EndChild();
					ImGui::PopStyleColor();
				}

			}
			if (tab == 1)
			{

				ImGui::PushStyleColor(ImGuiCol_ChildBg | ImGuiCol_Border, ImColor(8, 8, 8).Value);
				ImGui::BeginChild("", ImVec2(305, 300), false);
				{
					if (ImGui::Toggle("esp & tracers", &settings::visuals->boxes_show)) {
						g_array_list.toggle_module("    esp"), g_array_list.toggle_module("    tracers");
					}
					
					if (settings::visuals->boxes_show) {

						settings::visuals->tracer = true;

						ImGui::SameLine();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

						ImGui::Toggle("Fill", &settings::visuals->fill_boxes);

						if (settings::visuals->fill_boxes) {
							ImGui::SameLine();
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
							ImGui::ColorEdit4("", settings::visuals->boxes_fill_color, ImGuiColorEditFlags_NoInputs | ImGuiWindowFlags_NoCollapse);
						}


						ImGui::Dummy({ 0, 5.0f });


						if (ImGui::BeginCombo("", items[item]))
						{
							for (unsigned int i = 0; i < items_size; i++)
							{
								bool selected = (item == i);
								if (ImGui::Selectable(items[i], &selected))
									item = i;
								if (selected)
									ImGui::SetItemDefaultFocus();
							}

							ImGui::EndCombo();
						}


						if (item == 0) {
							settings::visuals->rainbow_boxes = true;
						}
						else {
							settings::visuals->rainbow_boxes = false;
						}

						ImGui::Dummy({ 0, 4.f });

						if (item == 1) {
							ImGui::Dummy({ 0, 4.f });
							ImGui::ColorEdit4("Boxes Color   ", settings::visuals->boxes_color, ImGuiColorEditFlags_NoInputs);
							ImGui::ColorEdit4("Tracer Color", &*(float*)settings::visuals->tracercolor, ImGuiColorEditFlags_NoInputs);
							ImGui::Dummy({ 0, 5.0f });
						}

						ImGui::Dummy({ 0, 10.f });
					}

					else {
						settings::visuals->tracer = false;
					}

					ImGui::SameLine();
					ImGui::Bind("##Blink bind", &settings::binds->espbindxd, ImVec2(60, 20));

					ImGui::Dummy({ 0, 10.f });
					ImGui::Toggle("xray", &settings::visuals->cavefinder);
						ImGui::SameLine();
						ImGui::Bind("##xray bind", &settings::binds->cavefinderbind, ImVec2(60, 20));
					
					ImGui::Dummy({ 0, 10.f });
					ImGui::Toggle("wireframe", &settings::visuals->wirefr);
						ImGui::SameLine();
						ImGui::Bind("##wire bind", &settings::binds->wirebind, ImVec2(60, 20));
				
					ImGui::Dummy({ 0, 10.f });
					ImGui::Toggle("clearesp", &settings::visuals->clearesp);
					ImGui::SameLine();
					ImGui::Bind("##clearesp bind", &settings::binds->clearbind, ImVec2(60, 20));
					ImGui::Dummy({ 0, 10.f });
					ImGui::Toggle("chams", &settings::visuals->esp);
					if (settings::visuals->esp) {
						ImGui::Dummy({ 0, 10.f });
						ImGui::Toggle("Colored", &settings::visuals->chams);
					}
						
					
					ImGui::SameLine();
					ImGui::Bind("##chams bind", &settings::binds->chamsbind, ImVec2(60, 20));
					ImGui::Dummy({ 0, 10.f });
					if (ImGui::Toggle("nametags", &settings::visuals->nametag_show)) {
						g_array_list.toggle_module("    nametags");
					}

					if (settings::visuals->nametag_show) {

						ImGui::Dummy({ 0, 4.0f });
						ImGui::SliderFloat("", &settings::visuals->nametagsize, 1.0, 5.0, "%f");

						ImGui::Dummy({ 0, 20.0f });

						ImGui::Toggle("Black Nickname", &settings::visuals->nametag_black);


					}
					ImGui::EndChild();
					ImGui::PopStyleColor();
				}



				

				ImGui::SameLine();

				
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 0);



				




				ImGui::Dummy({ 0, 5.0f }); //odstep pomiedzy buttonem

				ImGui::PushStyleColor(ImGuiCol_ChildBg | ImGuiCol_Border, ImColor(8, 8, 8).Value);
				ImGui::BeginChild("xd", ImVec2(300, 76), false);

				{
					
						
					
					

					ImGui::EndChild();
					ImGui::PopStyleColor();
				}
			}


			if (tab == 3) {
				{
					ImGui::PushStyleColor(ImGuiCol_ChildBg | ImGuiCol_Border, ImColor(8, 8, 8).Value);
					ImGui::BeginChild("xdd", ImVec2(259, 76), false);

					ImGui::Toggle("watermark", &settings::settings->arraylist);

					ImGui::Dummy({ 0, 6.0f });


	
					ImGui::EndChild();
					ImGui::PopStyleColor();
				}

			}

			
		}



		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
