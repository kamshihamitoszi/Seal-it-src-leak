#include "autoclicker.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdlib.h>
#include <fstream> 
#include <chrono>
#include <random>
#include <WinUser.h>
#include "../hooks/hooks.h"
long ostatniklik = 0;
int nastepnycps = 10;

void update() {
	if (!enabled) return;

	long milli = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if (ostatniklik == 0) ostatniklik = milli;
	if ((milli - ostatniklik) < (1000 / nastepnycps)) return;
	if (GetAsyncKeyState(VK_LBUTTON) && 1) {
		POINT pozycja;
		GetCursorPos(&pozycja);
		SendMessage(hwnd)
	}
}