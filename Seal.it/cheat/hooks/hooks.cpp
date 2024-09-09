#include <iostream>
#include <Windows.h>
#include <vector>
#include "hooks.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../libs/minhook/MinHook.h"
#include "../../cheat/gui/gui.h"
#include "../drawing/glDraw.h"
#include "../drawing/gltext.h"
#include "../../settings/settings.h"
#include "../array_list/array_list.h" 
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdlib.h>
#include <fstream> 
#include <chrono>
#include <thread>
#include <random>
#include "../../imgui_animated.h"
#define threadSleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));

class Position
{
public:
    std::vector <float> projection;
    std::vector <float> modelview;
};

template<class DataType>
class SpeedHack {
    DataType time_offset;
    DataType time_last_update;

    double speed_;

public:


    SpeedHack(DataType currentRealTime, double initialSpeed) {
        time_offset = currentRealTime;
        time_last_update = currentRealTime;

        speed_ = initialSpeed;
    }

    void setSpeed(DataType currentRealTime, double speed) {
        time_offset = getCurrentTime(currentRealTime);
        time_last_update = currentRealTime;

        speed_ = speed;
    }

    DataType getCurrentTime(DataType currentRealTime) {
        DataType difference = currentRealTime - time_last_update;

        return (DataType)(speed_ * difference) + time_offset;
    }
};

Position player;
GL::Font glFont;
GL::Font glFont2;

static unsigned char* FromHSB(float hue, float saturation, float brightness){
    float h = hue == 1.0f ? 0 : hue * 6.0f;
    float f = h - (int)h;
    float p = brightness * (1.0f - saturation);
    float q = brightness * (1.0f - saturation * f);
    float t = brightness * (1.0f - (saturation * (1.0f - f)));

    unsigned char* col = new unsigned char[3];

    if (h < 1)
    {
        col[0] = (unsigned char)(brightness * 255);
        col[1] = (unsigned char)(t * 255);
        col[2] = (unsigned char)(p * 255);
        return col;
    }
    else if (h < 2)
    {
        col[0] = (unsigned char)(q * 255);
        col[1] = (unsigned char)(brightness * 255);
        col[2] = (unsigned char)(p * 255);
        return col;
    }
    else if (h < 3)
    {
        col[0] = (unsigned char)(p * 255);
        col[1] = (unsigned char)(brightness * 255);
        col[2] = (unsigned char)(t * 255);
        return col;
    }
    else if (h < 4)
    {
        col[0] = (unsigned char)(p * 255);
        col[1] = (unsigned char)(q * 255);
        col[2] = (unsigned char)(brightness * 255);
        return col;
    }
    else if (h < 5)
    {
        col[0] = (unsigned char)(t * 255);
        col[1] = (unsigned char)(p * 255);
        col[2] = (unsigned char)(brightness * 255);
        return col;
    }
    else
    {
        col[0] = (unsigned char)(brightness * 255);
        col[1] = (unsigned char)(p * 255);
        col[2] = (unsigned char)(q * 255);
        return col;
    }
}

//========================================================================================//

typedef void(__stdcall* twglSwapBuffers)(HDC);
twglSwapBuffers owglSwapBuffers;


typedef BOOL(__stdcall* T_glScalef) (float, float, float);
T_glScalef pglScalef;

typedef BOOL(__stdcall* tglCallList) (GLuint list);

tglCallList oglCallList;
typedef DWORD(WINAPI* GetTickCountType)(void);
GetTickCountType   g_GetTickCountOriginal;
typedef ULONGLONG(WINAPI* GetTickCount64Type)(void);
GetTickCount64Type g_GetTickCount64Original;

typedef BOOL(WINAPI* QueryPerformanceCounterType)(LARGE_INTEGER* lpPerformanceCount);
GetTickCountType   g_TimeGetTimeOriginal;

int (WINAPI* pSend)(SOCKET s, const char* buf, int len, int flags) = send;
int WINAPI MySend(SOCKET s, const char* buf, int len, int flags);

//========================================================================================//

QueryPerformanceCounterType g_QueryPerformanceCounterOriginal;
SpeedHack<DWORD>     g_speedHack(GetTickCount(), settings::movement->Speedhack);
SpeedHack<ULONGLONG> g_speedHackULL(GetTickCount64(), settings::movement->Speedhack);
SpeedHack<LONGLONG>  g_speedHackLL(0, settings::movement->Speedhack);

void SetSpeed(double speed) {
    g_speedHack.setSpeed(g_GetTickCountOriginal(), speed);

    g_speedHackULL.setSpeed(g_GetTickCount64Original(), speed);

    LARGE_INTEGER performanceCounter;
    g_QueryPerformanceCounterOriginal(&performanceCounter);

    g_speedHackLL.setSpeed(performanceCounter.QuadPart, speed);
}

DWORD WINAPI GetTickCountHacked(void) {
    return g_speedHack.getCurrentTime(g_GetTickCountOriginal());
}

ULONGLONG WINAPI GetTickCount64Hacked(void) {
    return g_speedHackULL.getCurrentTime(g_GetTickCount64Original());
}

BOOL WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount) {
    LARGE_INTEGER performanceCounter;

    BOOL result = g_QueryPerformanceCounterOriginal(&performanceCounter);

    lpPerformanceCount->QuadPart = g_speedHackLL.getCurrentTime(performanceCounter.QuadPart);

    return result;
}

//========================================================================================//
BOOL __stdcall hglCallList(GLuint list) {
    if (settings::visuals->chams) {
        glColor3f(0.6, 0.917, 0.882);
        glDepthRange(1, 0);
        glDisable(GL_TEXTURE_2D);
        oglCallList(list);
        glDepthRange(0, 1);
        glEnable(GL_TEXTURE_2D);
    }
    else {
        glDepthRange(0, 1);
        glEnable(GL_TEXTURE_2D);
    }

    return oglCallList(list);

}

void tracers(Position& position){
    glLoadIdentity();
    if (settings::visuals->rainbow_boxes) {
        unsigned char color[3] = {
            FromHSB(settings::settings->rainbow, 1.f, 1.f)[0],
            FromHSB(settings::settings->rainbow, 1.f, 1.f)[1],
            FromHSB(settings::settings->rainbow, 1.f, 1.f)[2],
        };
        glColor4ub(color[0], color[1], color[2], 255.f);
    }
    else {
        glColor4fv(settings::visuals->tracercolor);
    }
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex3f(0, -0.1, - 19);



    glVertex3f((position.modelview.data())[12], (position.modelview.data())[13], (position.modelview.data())[14]);
    glEnd();
    glLineWidth(1);

    position.projection.clear();
    position.modelview.clear();
}

void savePosition(Position& position, float offsetX, float offsetY, float offsetZ){
    float projection[16]{};
    glGetFloatv(GL_PROJECTION_MATRIX, projection);

    float modelview[16]{};
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    float m3[4]{};

    for (int i = 0; i < 4; ++i)
    {
        m3[i] = modelview[i] * offsetX
            + modelview[i + 4] * offsetY
            + modelview[i + 8] * offsetZ
            + modelview[i + 12];
    }

    memcpy(modelview + 12, m3, sizeof(m3));

    position.projection.insert(position.projection.end(), std::begin(projection), std::end(projection));
    position.modelview.insert(position.modelview.end(), std::begin(modelview), std::end(modelview));
    
   

    if (!menuShown) {
        if (settings::combat->aimassist_show) {
            for (int i = 0; i < position.modelview.size(); i += 16)
            {
                glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
                float distanceplayer(modelview[12] * modelview[12] + modelview[13] * modelview[13] + modelview[14] * modelview[14]);

                distanceplayer = distanceplayer / 4;

                if (distanceplayer < settings::combat->aimassist_distance) {
                        mouse_event(MOUSEEVENTF_MOVE, (position.modelview.data() + i)[12] * settings::combat->aimassist_speed * 7 + 1, 0, 0, 0);
                        mouse_event(MOUSEEVENTF_MOVE, (position.modelview.data() + i)[12] * settings::combat->aimassist_speed * 7 - 1, 0, 0, 0);
                    }
                }
            position.projection.clear();
            position.modelview.clear();
        }
    }
}


void color_thread( ) {
    while ( !GetAsyncKeyState( VK_END ) ) {
        if ( !settings::settings->going_down ) {
            settings::settings->rainbow += 0.005f;
            if (settings::settings->rainbow > 1.f ) {
                settings::settings->going_down = true;
            }
        }
        else {
            settings::settings->rainbow -= 0.005f;
            if (settings::settings->rainbow < 0.005f ) {
                settings::settings->going_down = false;
            }
        }
        Sleep( 20 );
    }
}

void __stdcall hooked_wgl_swap_buffers(HDC hdc) {

    if (!hdc)
        return owglSwapBuffers(hdc);

   
    if (GetAsyncKeyState(settings::binds->speedhackbind) & 1) {
        settings::movement->speedhack_show ^= 1;
        g_array_list.toggle_module("    speedhack");
    }


    if ( GetAsyncKeyState( settings::binds->cavefinderbind ) & 1 ) {
        settings::visuals->cavefinder ^= 1;
        g_array_list.toggle_module( "   Cavefinder" );
    }if ( GetAsyncKeyState( settings::binds->wirebind ) & 1 ) {
        settings::visuals->wirefr ^= 1;
        g_array_list.toggle_module( "   wirefr" );
    }if ( GetAsyncKeyState( settings::binds->clearbind ) & 1 ) {
        settings::visuals->clearesp ^= 1;
        g_array_list.toggle_module( "   clearesp" );
    }if ( GetAsyncKeyState( settings::binds->chamsbind ) & 1 ) {
        settings::visuals->esp ^= 1;
        settings::visuals->chams ^= 1;
        g_array_list.toggle_module( "   chams" );
    }


    if (GetAsyncKeyState(settings::binds->espbindxd) & 1) {
        settings::visuals->esp ^= 1;
        settings::visuals->boxes_show ^= 1;
        settings::visuals->tracer ^= 1;
        g_array_list.toggle_module("    esp");
    }

    if ( GetAsyncKeyState( settings::binds->aimassistbind ) & 1 ) {
        settings::combat->aimassist_show ^= 1;
        g_array_list.toggle_module("    aimassist");
    }

    if (GetAsyncKeyState(settings::binds->blinkbind) & 1) {
        settings::misc->blink_show ^= 1;
        g_array_list.toggle_module("    blink");
    }

    if (hwnd != WindowFromDC(hdc)) {
        hwnd = WindowFromDC(hdc);
        if (!hwnd) return owglSwapBuffers(hdc);
        Gui::Get()->ImGuiInit();
    }
    
    HDC currentHDC = wglGetCurrentDC();
    if (!glFont.bBuilt || currentHDC != glFont.hdc) {
        glFont.Build( 29  );
    }
    if ( !glFont2.bBuilt || currentHDC != glFont2.hdc ) {
        glFont2.Build( 20 );
    }

    auto draw_running_module = [ ] ( const char* module_name, float& rainbow, int& enabled_amount, bool going_down ) -> void {
        for ( int i = 0; i < 10; i++ ) {
            if ( !going_down ) {
                rainbow += 0.005f;
                if ( rainbow > 1.f ) {
                    going_down = true;
                }
            }
            else {
                rainbow -= 0.005f;
                if ( rainbow < 0.005f ) {
                    going_down = false;
                }
            }
        }
        glFont2.Print( 3, 50 + ( enabled_amount * 25 ), FromHSB( rainbow, 1.f, 1.f ), module_name );
        enabled_amount++;
    };
    if (settings::visuals->cavefinder) {
        //glGetFloatv(GL_CURRENT_COLOR, curcolor);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        //glColor4f(curcolor[0], curcolor[1], curcolor[2], 1.0);

    }
    else {
        //glGetFloatv(GL_CURRENT_COLOR, curcolor);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        // glPopMatrix();
        // glColor4f(curcolor[0], curcolor[1], curcolor[2], 1.0);
    }
    if (settings::visuals->wirefr) {
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }
    if (settings::visuals->clearesp) {

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glCullFace(GL_FRONT_AND_BACK);

        //
    }
    else {
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);

    }
    GL::SetupOrtho();
    {
        Gui::Get()->Render();

        glPushAttrib(GL_CURRENT_BIT);
        glColor3f(0, 0, 0);

        if (settings::settings->arraylist) {
            glFont.Print(3, 25, FromHSB(settings::settings->rainbow, 1.f, 1.f), "   unnsense.cc");
        }

        int enabled_amount = 0;
        float new_rainbow = settings::settings->rainbow;
        for ( auto actual_module : g_array_list.get_enabled_modules( ) ) {
            if (settings::settings->arraylist) {
                draw_running_module(actual_module.c_str(), new_rainbow, enabled_amount, settings::settings->going_down);
            }
        }

        glPopAttrib();
    }
    GL::RestoreGL();

    if (settings::movement->speedhack_show) {
        SetSpeed(settings::movement->SpeedhackSpeed);
    }
    else {
        SetSpeed(1);
    }

    return owglSwapBuffers(hdc);
}



void __stdcall myglScalef(float x, float y, float z) {
    pglScalef(x, y, z);


    if (settings::visuals->nametag_black) {
        glBlendFunc(GL_ONE, GL_ZERO);
    }

   
        if (x == 0.00390625) {
            if (settings::visuals->cavefinder) {
                glDisable(GL_DEPTH_TEST);
            }
        }
    

    if (x == -0.026666669175028801) {
        
        glScalef(settings::visuals->nametagsize, settings::visuals->nametagsize, settings::visuals->nametagsize);
    }

    if (x > 0.9f && x < 1.f) {
        savePosition(player, 0.0F, -1.0F, 0.0F);

        if (settings::visuals->tracer) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix(); {
                glEnable(GL_LINE_SMOOTH);
                glDisable(GL_TEXTURE_2D);
                glDisable(GL_CULL_FACE);
                glDisable(GL_LIGHTING);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_LIGHTING);
                glDisable(GL_DEPTH_TEST);

                tracers(player);
            }
            glPopAttrib();
            glPopMatrix();
        }


        if (settings::visuals->esp) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0F, -1100000.0F);
            glDisable(GL_LIGHTING);
        }


        if (settings::visuals->boxes_show) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix();

            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);

            float z_to_move = 0.5;
            float height = 2;

            if (settings::visuals->fill_boxes) {
                glColor4f(settings::visuals->boxes_fill_color[0], settings::visuals->boxes_fill_color[1], settings::visuals->boxes_fill_color[2], settings::visuals->boxes_fill_color[3]);

                //filling
                glBegin(GL_QUADS);
                {
                    if (settings::visuals->boxes_3d) {
                        //front
                        glVertex3f(-0.5, 0.05, -z_to_move);
                        glVertex3f(0.5, 0.05, -z_to_move);
                        glVertex3f(0.5, -height, -z_to_move);
                        glVertex3f(-0.5, -height, -z_to_move);

                        //back
                        glVertex3f(-0.5, 0.05, z_to_move);
                        glVertex3f(0.5, 0.05, z_to_move);
                        glVertex3f(0.5, -height, z_to_move);
                        glVertex3f(-0.5, -height, z_to_move);

                        //left
                        glVertex3f(0.495, 0.05, 0.5);
                        glVertex3f(0.495, 0.05, -0.5);
                        glVertex3f(0.495, -height, -0.5);
                        glVertex3f(0.495, -height, 0.5);

                        //right
                        glVertex3f(-0.495, 0.05, 0.5);
                        glVertex3f(-0.495, 0.05, -0.5);
                        glVertex3f(-0.495, -height, -0.5);
                        glVertex3f(-0.495, -height, 0.5);

                        //top
                        glVertex3f(0.495, -height, 0.5);
                        glVertex3f(0.495, -height, -0.5);
                        glVertex3f(-0.495, -height, 0.5);
                        glVertex3f(-0.495, -height, -0.5);

                        //bottom
                        glVertex3f(0.495, 0.05, 0.5);
                        glVertex3f(0.495, 0.05, -0.5);
                        glVertex3f(-0.495, 0.05, 0.5);
                        glVertex3f(-0.495, 0.05, -0.5);
                    }
                }
                glEnd();
            }

            glLineWidth(1);
            if (settings::visuals->rainbow_boxes) {
                unsigned char color[3] = { 
                    FromHSB(settings::settings->rainbow, 1.f, 1.f)[0],
                    FromHSB(settings::settings->rainbow, 1.f, 1.f)[1],
                    FromHSB(settings::settings->rainbow, 1.f, 1.f)[2],
                };
                glColor4ub(color[0], color[1], color[2], 255.f);
            }
            else {
                glColor4f(settings::visuals->boxes_color[0], settings::visuals->boxes_color[1], settings::visuals->boxes_color[2], settings::visuals->boxes_color[3]);
            }

            //full box
            glBegin(GL_LINES);
            {
                if (settings::visuals->boxes_3d) {
                    //front bottom
                    glVertex3f(-0.5, 0.05, -z_to_move);
                    glVertex3f(0.5, 0.05, -z_to_move);

                    //back bottom
                    glVertex3f(-0.5, 0.05, z_to_move);
                    glVertex3f(0.5, 0.05, z_to_move);

                    //left bottom
                    glVertex3f(0.495, 0.05, 0.5);
                    glVertex3f(0.495, 0.05, -0.5);

                    //right bottom
                    glVertex3f(-0.495, 0.05, 0.5);
                    glVertex3f(-0.495, 0.05, -0.5);

                    //front top
                    glVertex3f(-0.5, -height, -z_to_move);
                    glVertex3f(0.5, -height, -z_to_move);

                    //back top
                    glVertex3f(-0.5, -height, z_to_move);
                    glVertex3f(0.5, -height, z_to_move);

                    //left top
                    glVertex3f(0.495, -height, 0.5);
                    glVertex3f(0.495, -height, -0.5);

                    //right top
                    glVertex3f(-0.495, -height, 0.5);
                    glVertex3f(-0.495, -height, -0.5);

                    //right front connection
                    glVertex3f(-0.495, -height, -0.5);
                    glVertex3f(-0.495, 0.05, -0.5);

                    //right back connection
                    glVertex3f(-0.495, -height, 0.5);
                    glVertex3f(-0.495, 0.05, 0.5);

                    //left back connection
                    glVertex3f(0.495, -height, 0.5);
                    glVertex3f(0.495, 0.05, 0.5);

                    //left front connection
                    glVertex3f(0.495, -height, -0.5);
                    glVertex3f(0.495, 0.05, -0.5);
                }
            }
            glEnd();

            glLineWidth(1);

            glPopMatrix();
            glPopAttrib();
        }
    }
}


int __stdcall MySend(SOCKET s, const char* buf, int len, int flags){

    return  settings::misc->blink_show ? 0 : pSend(s, buf, len, flags);
}


void hooks::Initalize(){

    MH_Initialize();

    HMODULE hMod = GetModuleHandle("opengl32.dll");
    HMODULE kernel32 = GetModuleHandle("Kernel32.dll");
    HMODULE winmm = GetModuleHandle("Winmm.dll");
    HMODULE WS2 = GetModuleHandle("WS2_32.dll");

    void* ptr = GetProcAddress(hMod, "wglSwapBuffers");
    void* xd = GetProcAddress(hMod, "glScalef");
    void* send_packet = GetProcAddress(WS2, "WSASend");
    void* speed = GetProcAddress(kernel32, "GetTickCount");
    void* speed1 = GetProcAddress(kernel32, "GetTickCount64");
    void* speed2 = GetProcAddress(kernel32, "timeGetTime");
    void* speed3 = GetProcAddress(kernel32, "QueryPerformanceCounter");
    void* call = GetProcAddress(hMod, "glCallList");
    MH_CreateHook(send_packet, MySend, (void**)&pSend);
    MH_CreateHook(xd, myglScalef, (void**)&pglScalef);

    MH_CreateHook(ptr, hooked_wgl_swap_buffers, (void**)&owglSwapBuffers);
    MH_CreateHook(speed, GetTickCountHacked, (void**)&g_GetTickCountOriginal);
    MH_CreateHook(speed1, GetTickCount64Hacked, (void**)&g_GetTickCount64Original);
    MH_CreateHook(speed2, GetTickCountHacked, (void**)&g_TimeGetTimeOriginal);

    MH_CreateHook(call, hglCallList, reinterpret_cast<void**>(&oglCallList));
    MH_CreateHook(speed3, QueryPerformanceCounterHacked, (void**)&g_QueryPerformanceCounterOriginal);

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)color_thread, 0, 0, 0);

    MH_EnableHook(MH_ALL_HOOKS);
    MH_ApplyQueued();
}

void hooks::UnInitalize() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}


