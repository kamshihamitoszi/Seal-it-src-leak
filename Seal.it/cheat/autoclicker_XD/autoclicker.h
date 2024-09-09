#pragma once
struct autoclicker_lewyok
{
	inline static bool enabled = false;

	inline static float lewymaxcps = 15;
	inline static float lewymincps = 8;

	static void update();
	static void renderujmenuok();

};