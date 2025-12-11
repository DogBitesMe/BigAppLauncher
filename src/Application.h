#pragma once

#include "graphics/DX11Context.h"
#include "graphics/VideoPlayer.h"
#include "graphics/BlurEffect.h"
#include "ui/screens/LoginScreen.h"
#include "ui/screens/ProductsScreen.h"
#include "ui/screens/GUIMenuScreen.h"
#include "ui/DebugController.h"
#include "ui/HUDOverlay.h"
#include <Windows.h>
#include <string>
#include <memory>

class Application {
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int width, int height, const wchar_t* title);
    int Run();
    void Shutdown();

    // Window message handler
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    bool InitializeImGui();
    void ShutdownImGui();
    bool InitializeVideoBackground();
    bool InitializeBlurEffect();
    void RenderVideoBackground();
    void RenderBlurredPanelBackground(float x, float y, float width, float height);
    void RenderWindowControls();

    void Update();
    void Render();

    void OnResize(int width, int height);

    // Window
    HWND m_hwnd = nullptr;
    HINSTANCE m_hInstance = nullptr;
    int m_width = 0;
    int m_height = 0;
    bool m_running = true;

    // Graphics
    std::unique_ptr<DX11Context> m_dx11;
    std::unique_ptr<VideoPlayer> m_videoPlayer;
    std::unique_ptr<BlurEffect> m_blurEffect;

    // Screens
    std::unique_ptr<LoginScreen> m_loginScreen;
    std::unique_ptr<ProductsScreen> m_productsScreen;
    std::unique_ptr<GUIMenuScreen> m_guiMenuScreen;
    std::unique_ptr<HUDOverlay> m_hudOverlay;

    // Debug
    std::unique_ptr<DebugController> m_debugController;

    // State
    enum class AppState {
        Login,
        Products,
        GUIMenu
    };
    AppState m_state = AppState::Login;

    // Timing
    LARGE_INTEGER m_lastTime = {};
    LARGE_INTEGER m_frequency = {};

    // Debug info
    std::wstring m_videoStatus;

    // Window controls config
    bool m_showMaximizeButton = true;
};
