#include windows.h
#include iostream

 Физическая структура векторов движка Irrlicht Engine
struct Vector3df {
    float x;  Направление влево  вправо (Strafe)
    float y;  Высота персонажа (Vertical)
    float z;  Направление вперед  назад (Forward)
};

 Примерная структура компонентов PlayerController в памяти
struct PlayerController {
    unsigned char padding0[0x20];  Пропуск внутренних переменных движка
    Vector3df position;            Текущие координаты игрока в мире
    unsigned char padding1[0x10];  Пропуск до следующего блока данных
    bool isGrounded;               Проверка касания земли (1 - на земле, 0 - в воздухе)
    float movementSpeedScalar;     Базовый множитель скорости перемещения
};

 Смещение структуры игрока относительно главного модуля игры (Bugsnax.exe)
 Этот базовый адрес динамически обновляется при запуске
uintptr_t GetPlayerControllerAddress() {
    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
     При компиляции через VS здесь настраивается точный указатель (Pointer Offset)
    uintptr_t playerOffset = 0x00A1B2C3; 
    return (baseAddress + playerOffset);
}

 Основной поток работы нашего мультихака
DWORD WINAPI MultiHackMain(LPVOID lpParam) {
     Вызов отладочной консоли внутри игры при инжекте библиотеки
    AllocConsole();
    FILE consoleStream;
    freopen_s(&consoleStream, CONOUT$, w, stdout);
    
    stdcout  === Bugsnax Irrlicht MultiHack Source v1.0 ===n;
    stdcout  [F1] Toggle Fly Moden;
    stdcout  [F2] Toggle Infinite Jumpn;
    stdcout  [F3] Toggle Speed Hack (4.0x)n;
    stdcout  [END] Unload Hacknn;

    bool flyModeActive = false;
    bool infJumpActive = false;
    bool speedHackActive = false;

     Бесконечный цикл опроса клавиш во время игры
    while (!GetAsyncKeyState(VK_END)) {
        PlayerController pController = (PlayerController)GetPlayerControllerAddress();

        if (pController != nullptr) {
            
             1. Управление функцией FLY MODE (Замораживаем Y ось и даем летать)
            if (GetAsyncKeyState(VK_F1) & 1) {
                flyModeActive = !flyModeActive;
                stdcout  [Fly Mode]   (flyModeActive  ENABLED  DISABLED)  n;
            }
            if (flyModeActive) {
                 Если зажат Пробел — летим вверх, если Shift — плавно спускаемся вниз
                if (GetAsyncKeyState(VK_SPACE)) pController-position.y += 0.5f;
                if (GetAsyncKeyState(VK_SHIFT)) pController-position.y -= 0.5f;
            }

             2. Управление функцией INFINITE JUMP (Обход флага земли)
            if (GetAsyncKeyState(VK_F2) & 1) {
                infJumpActive = !infJumpActive;
                stdcout  [Infinite Jump]   (infJumpActive  ENABLED  DISABLED)  n;
            }
            if (infJumpActive) {
                 Движок всегда думает, что игрок стоит на твердой поверхности
                pController-isGrounded = true; 
            }

             3. Управление функцией SPEED HACK (Множитель скорости перемещения)
            if (GetAsyncKeyState(VK_F3) & 1) {
                speedHackActive = !speedHackActive;
                 При активации умножаем скорость в 4 раза, при выключении возвращаем 1.0x
                pController-movementSpeedScalar = speedHackActive  4.0f  1.0f;
                stdcout  [Speed Hack]   (speedHackActive  ENABLED  DISABLED)  n;
            }
        }
        Sleep(10);  Снижаем нагрузку на процессор
    }

     Корректное закрытие консоли и выгрузка DLL из памяти игры
    stdcout  Unloading MultiHack...n;
    if (consoleStream) fclose(consoleStream);
    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
    return 0;
}

 Главная точка входа динамической библиотеки (DllMain)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
         Создаем отдельный независимый поток в игре, чтобы процесс не завис
        CreateThread(NULL, 0, MultiHackMain, hModule, 0, NULL);
    }
    return TRUE;
}
