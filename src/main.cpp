#include "Application.h"
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    // Enable high DPI awareness
    SetProcessDPIAware();

    Application app;

    if (!app.Initialize(hInstance, 1280, 720, L"BigAppLauncher")) {
        MessageBoxW(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    int result = app.Run();

    app.Shutdown();

    return result;
}
