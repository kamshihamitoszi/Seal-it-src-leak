#pragma once
struct Visuals {

	int boxes_3d = 1;
	bool esp = false;
	bool cavefinder = false;
	bool fill_boxes = true;
	bool tracer = false;
	bool boxes_show = false;
	bool xray_show = false;
	bool wirefr = false;
	bool clearesp = false;
	bool chams = false;
	bool nametag_show = false;
	bool nametag_black = false;
	float nametagsize = 1;
	bool rainbow_boxes;
	bool rainbow_tracers;
	float tracercolor[4] = { 0, 1, 0.462, 1.0 };
	float boxes_color[4] = { 0, 1, 0.462, 1.0 };
	float boxes_fill_color[4] = { 0.05, 0.05, 0.05, 0.3f };
};

struct LeftAutoClicker
{
	inline static bool Enabled = false;

	inline static float leftMaxCps = 14;
	inline static float leftMinCps = 8;
	inline static bool ignoreBlocks = true;

	static void Update();
	static void RenderMenu();
};
struct Combat {

	bool aimassist_show = false;
	float aimassist_distance = 1;
	float aimassist_speed = 0.05f;
};


struct Movement {
   bool speedhack_show = false;
   double Speedhack = 1.0;
   float SpeedhackSpeed = 1;
};

struct Binds {

	int  espbind{ 0 };
	int  clearbind{ 0 };
	int  chamsbind{ 0 };
	int  wirebind{ 0 };
	int  espbindxd{ 0 };
	int  cavefinderbind{ 0 };
	int  aimassistbind{ 0 };
	int  speedhackbind{ 0 };
	int  blinkbind { 0 };
};

struct Misc {
	bool blink_show = false;
};


struct Settings {
	bool arraylist = true;
	float rainbow;
	bool going_down = false;
	bool gui = false;

	bool config = false;

};


struct Category {
	bool movement_c = true;
	bool visuals_c = true;
	bool combat_c = true;
	bool misc_c = true;
	bool settings_c = true;
};


namespace settings {
	inline Visuals* visuals = new Visuals( );
	inline Combat* combat = new Combat( );
	inline Movement* movement = new Movement( );
	inline Category* category = new Category( );
	inline Binds* binds = new Binds( );
	inline Settings* settings = new Settings();
	inline Misc* misc = new Misc();
}