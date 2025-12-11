#include "Application.h"
#include "ui/Theme.h"
#include "i18n/Localization.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() {
    m_dx11 = std::make_unique<DX11Context>();
    m_videoPlayer = std::make_unique<VideoPlayer>();
    m_loginScreen = std::make_unique<LoginScreen>();
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize(HINSTANCE hInstance, int width, int height, const wchar_t* title) {
    m_hInstance = hInstance;
    m_width = width;
    m_height = height;

    // Initialize timing
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_lastTime);

    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"BigAppLauncherClass";

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    // Calculate window size to get desired client area
    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create window
    m_hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, this
    );

    if (!m_hwnd) {
        return false;
    }

    // Initialize DirectX 11
    if (!m_dx11->Initialize(m_hwnd, width, height)) {
        return false;
    }

    // Initialize ImGui
    if (!InitializeImGui()) {
        return false;
    }

    // Initialize video background
    InitializeVideoBackground();

    // Load default language
    i18n::Localization::Instance().SetBasePath("assets/lang");
    i18n::SetLanguage("zh-CN");

    // Show window
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    return true;
}

bool Application::InitializeImGui() {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Disable imgui.ini
    io.IniFilename = nullptr;

    // Setup platform/renderer backends
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_dx11->GetDevice(), m_dx11->GetContext());

    // Load fonts - try multiple paths
    const char* fontPaths[] = {
        "assets/fonts/NotoSansSC-Regular.otf",
        "assets/fonts/NotoSansSC-Regular.ttf",
        "assets/fonts/NotoSansCJKsc-Regular.otf"
    };

    ImFont* font = nullptr;
    for (const char* fontPath : fontPaths) {
        font = io.Fonts->AddFontFromFileTTF(
            fontPath,
            18.0f,
            nullptr,
            io.Fonts->GetGlyphRangesChineseFull()
        );
        if (font) break;
    }

    if (!font) {
        // Fallback to default font
        io.Fonts->AddFontDefault();
    }

    // Apply custom theme
    Theme::Apply();

    return true;
}

bool Application::InitializeVideoBackground() {
    if (!m_videoPlayer->Initialize(m_dx11->GetDevice(), m_dx11->GetContext())) {
        m_videoStatus = L"Video init failed";
        return false;
    }

    // Get executable directory for relative path resolution
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/") + 1);

    std::wstring videoPath = exeDir + L"assets\\videos\\login_bg.mp4";
    m_videoStatus = L"Loading: " + videoPath;

    // Try to load video
    if (m_videoPlayer->LoadVideo(videoPath)) {
        m_videoPlayer->SetLoop(true);
        m_videoPlayer->Play();
        m_videoStatus = L"Playing: " + std::to_wstring(m_videoPlayer->GetWidth()) + L"x" + std::to_wstring(m_videoPlayer->GetHeight());
        return true;
    }

    m_videoStatus = L"Load failed: " + videoPath;
    return false;
}

void Application::RenderVideoBackground() {
    if (!m_videoPlayer || !m_videoPlayer->IsLoaded()) {
        return;
    }

    auto* srv = m_videoPlayer->GetTextureSRV();
    if (!srv) {
        return;
    }

    // Calculate aspect-ratio preserving size (cover mode - fill entire screen)
    float videoAspect = (float)m_videoPlayer->GetWidth() / (float)m_videoPlayer->GetHeight();
    float windowAspect = (float)m_width / (float)m_height;

    float drawWidth, drawHeight;
    float offsetX = 0, offsetY = 0;

    if (windowAspect > videoAspect) {
        // Window is wider - fit to width (video will be cropped top/bottom)
        drawWidth = (float)m_width;
        drawHeight = drawWidth / videoAspect;
        offsetY = ((float)m_height - drawHeight) * 0.5f;
    } else {
        // Window is taller - fit to height (video will be cropped left/right)
        drawHeight = (float)m_height;
        drawWidth = drawHeight * videoAspect;
        offsetX = ((float)m_width - drawWidth) * 0.5f;
    }

    // Draw video directly to background draw list (renders behind all windows)
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    drawList->AddImage(
        (ImTextureID)srv,
        ImVec2(offsetX, offsetY),
        ImVec2(offsetX + drawWidth, offsetY + drawHeight)
    );
}

void Application::ShutdownImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

int Application::Run() {
    MSG msg = {};

    while (m_running) {
        // Process messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                m_running = false;
            }
        }

        if (!m_running) {
            break;
        }

        // Update and render
        Update();
        Render();
    }

    return static_cast<int>(msg.wParam);
}

void Application::Update() {
    // Calculate delta time
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    float deltaTime = static_cast<float>(currentTime.QuadPart - m_lastTime.QuadPart) /
                      static_cast<float>(m_frequency.QuadPart);
    m_lastTime = currentTime;

    // Update video
    if (m_videoPlayer && m_videoPlayer->IsPlaying()) {
        m_videoPlayer->Update(deltaTime);
    }

    // Update logic
    if (m_loginScreen && m_loginScreen->IsLoggedIn()) {
        m_state = AppState::Products;
    }
}

void Application::Render() {
    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Clear background with dark color
    m_dx11->BeginFrame(0.06f, 0.06f, 0.09f, 1.0f);

    // Render video background first (only on login screen)
    if (m_state == AppState::Login) {
        RenderVideoBackground();
    }

    // Render current screen
    switch (m_state) {
        case AppState::Login:
            if (m_loginScreen) {
                m_loginScreen->Render(m_width, m_height);
            }
            break;

        case AppState::Products:
            ImGui::SetNextWindowPos(ImVec2(m_width * 0.5f, m_height * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if (ImGui::Begin("Products", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
                ImGui::Text("Product Screen - Coming Soon!");
            }
            ImGui::End();
            break;
    }


    // Render ImGui
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    m_dx11->EndFrame();
}

void Application::OnResize(int width, int height) {
    if (width > 0 && height > 0) {
        m_width = width;
        m_height = height;
        m_dx11->Resize(width, height);
    }
}

void Application::Shutdown() {
    if (m_videoPlayer) {
        m_videoPlayer->Shutdown();
    }

    ShutdownImGui();
    m_dx11->Cleanup();

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Handle ImGui messages
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    Application* app = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        app = reinterpret_cast<Application*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    } else {
        app = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (msg) {
        case WM_SIZE:
            if (app && wParam != SIZE_MINIMIZED) {
                app->OnResize(LOWORD(lParam), HIWORD(lParam));
            }
            return 0;

        case WM_SYSCOMMAND:
            // Disable ALT menu
            if ((wParam & 0xFFF0) == SC_KEYMENU) {
                return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
