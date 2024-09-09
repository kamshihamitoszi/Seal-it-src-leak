#include "../../libs/imgui/imgui.h"

extern bool menuShown;

class Gui {
private:
	void init_style();
	ImFont* font1;
	ImFont* font2;

public:
	static Gui* Get();
	void ImGuiInit();
	void ImGuiUnInit();
	void InitFonts();
	void Render();
};
