#include <iostream>
#include <Windows.h>
#include "cheat/gui/gui.h"
#include "cheat/hooks/hooks.h"

using namespace std;

#define SealItEntry DWORD

SealItEntry HackThread(HMODULE hModule){
        hooks::Initalize();

        
        FILE* f;


        while (!GetAsyncKeyState(VK_END)) {
            Sleep(10);
        }

        hooks::UnInitalize();
        Gui::Get()->ImGuiUnInit();
        FreeLibraryAndExitThread(hModule, 0);

        return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
    }
    return TRUE;
}