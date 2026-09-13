#include <windows.h>

struct Vector3df {
    float x;
    float y;
    float z;
};

struct PlayerController {
    unsigned char padding0[0x20];
    Vector3df position;
    unsigned char padding1[0x10];
    bool isGrounded;
    float movementSpeedScalar;
};

DWORD WINAPI MultiHackMain(LPVOID lpParam) {
    bool flyModeActive = false;
    bool infJumpActive = false;
    bool speedHackActive = false;

    while (!GetAsyncKeyState(VK_END)) {
        uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
        uintptr_t playerOffset = 0x00A1B2C3; // Базовый адрес для теста
        PlayerController* pController = (PlayerController*)(baseAddress + playerOffset);

        if (pController != nullptr) {
            // F1 - Летать
            if (GetAsyncKeyState(VK_F1) & 1) {
                flyModeActive = !flyModeActive;
            }
            if (flyModeActive) {
                if (GetAsyncKeyState(VK_SPACE)) pController->position.y += 0.5f;
                if (GetAsyncKeyState(VK_SHIFT)) pController->position.y -= 0.5f;
            }

            // F2 - Бесконечный прыжок
            if (GetAsyncKeyState(VK_F2) & 1) {
                infJumpActive = !infJumpActive;
            }
            if (infJumpActive) {
                pController->isGrounded = true; 
            }

            // F3 - Скорость
            if (GetAsyncKeyState(VK_F3) & 1) {
                speedHackActive = !speedHackActive;
                pController->movementSpeedScalar = speedHackActive ? 4.0f : 1.0f;
            }
        }
        Sleep(10);
    }

    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, MultiHackMain, hModule, 0, NULL);
    }
    return TRUE;
}
