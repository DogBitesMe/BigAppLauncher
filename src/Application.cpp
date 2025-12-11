#include "Application.h"
#include "ui/Theme.h"
#include "ui/IconsFontAwesome6.h"
#include "ui/DebugController.h"
#include "i18n/Localization.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <dwmapi.h>
#include <windowsx.h>

#pragma comment(lib, "dwmapi.lib")

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application() {
    m_dx11 = std::make_unique<DX11Context>();
    m_videoPlayer = std::make_unique<VideoPlayer>();
    m_blurEffect = std::make_unique<BlurEffect>();
    m_loginScreen = std::make_unique<LoginScreen>();
    m_productsScreen = std::make_unique<ProductsScreen>();
    m_guiMenuScreen = std::make_unique<GUIMenuScreen>();
    m_hudOverlay = std::make_unique<HUDOverlay>();
    m_debugController = std::make_unique<DebugController>();
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

    // Create borderless window
    DWORD style = WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
    DWORD exStyle = WS_EX_APPWINDOW;

    // Calculate window size
    RECT rect = { 0, 0, width, height };
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    // Get screen center position
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // Create window
    m_hwnd = CreateWindowExW(
        exStyle,
        wc.lpszClassName,
        title,
        style,
        posX, posY,
        windowWidth, windowHeight,
        nullptr, nullptr, hInstance, this
    );

    if (!m_hwnd) {
        return false;
    }

    // Enable rounded corners on Windows 11
    DWORD cornerPreference = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(m_hwnd, 33, &cornerPreference, sizeof(cornerPreference)); // DWMWA_WINDOW_CORNER_PREFERENCE = 33

    // Extend frame into client area for shadow effect
    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(m_hwnd, &margins);

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

    // Initialize blur effect
    InitializeBlurEffect();

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
        font = io.Fonts->AddFontDefault();
    }

    // Load FontAwesome icons and merge with main font
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(
        "assets/fonts/fa-solid-900.ttf",
        14.0f,
        &icons_config,
        icons_ranges
    );

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

bool Application::InitializeBlurEffect() {
    if (!m_blurEffect) return false;
    return m_blurEffect->Initialize(m_dx11->GetDevice(), m_dx11->GetContext(), m_width, m_height);
}

void Application::RenderBlurredPanelBackground(float x, float y, float width, float height) {
    if (!m_blurEffect || !m_videoPlayer || !m_videoPlayer->IsLoaded()) return;

    // Get video texture
    auto* videoSRV = m_videoPlayer->GetTextureSRV();
    if (!videoSRV) return;

    // Apply blur to the video texture
    auto* blurredSRV = m_blurEffect->Apply(videoSRV, 2.5f);
    if (!blurredSRV) return;

    // Reset render target after blur effect (it changes the render target)
    ID3D11RenderTargetView* rtv = m_dx11->GetRenderTargetView();
    m_dx11->GetContext()->OMSetRenderTargets(1, &rtv, nullptr);

    // Calculate UV coordinates for the panel region (based on video aspect ratio)
    float videoAspect = (float)m_videoPlayer->GetWidth() / (float)m_videoPlayer->GetHeight();
    float windowAspect = (float)m_width / (float)m_height;

    float videoDrawWidth, videoDrawHeight;
    float videoOffsetX = 0, videoOffsetY = 0;

    if (windowAspect > videoAspect) {
        videoDrawWidth = (float)m_width;
        videoDrawHeight = videoDrawWidth / videoAspect;
        videoOffsetY = ((float)m_height - videoDrawHeight) * 0.5f;
    } else {
        videoDrawHeight = (float)m_height;
        videoDrawWidth = videoDrawHeight * videoAspect;
        videoOffsetX = ((float)m_width - videoDrawWidth) * 0.5f;
    }

    // Map panel coordinates to video UV space
    float u0 = (x - videoOffsetX) / videoDrawWidth;
    float v0 = (y - videoOffsetY) / videoDrawHeight;
    float u1 = (x + width - videoOffsetX) / videoDrawWidth;
    float v1 = (y + height - videoOffsetY) / videoDrawHeight;

    // Clamp UVs
    u0 = (u0 < 0) ? 0 : (u0 > 1 ? 1 : u0);
    v0 = (v0 < 0) ? 0 : (v0 > 1 ? 1 : v0);
    u1 = (u1 < 0) ? 0 : (u1 > 1 ? 1 : u1);
    v1 = (v1 < 0) ? 0 : (v1 > 1 ? 1 : v1);

    // Draw blurred region as panel background
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    drawList->AddImageRounded(
        (ImTextureID)blurredSRV,
        ImVec2(x, y),
        ImVec2(x + width, y + height),
        ImVec2(u0, v0),
        ImVec2(u1, v1),
        IM_COL32(255, 255, 255, 255),
        Theme::Size::Rounding
    );

    // Add a subtle tint overlay for the frosted glass effect
    drawList->AddRectFilled(
        ImVec2(x, y),
        ImVec2(x + width, y + height),
        IM_COL32(15, 15, 25, 160),
        Theme::Size::Rounding
    );

    // Add subtle border glow
    drawList->AddRect(
        ImVec2(x, y),
        ImVec2(x + width, y + height),
        IM_COL32(255, 255, 255, 30),
        Theme::Size::Rounding,
        0,
        1.5f
    );
}

void Application::RenderWindowControls() {
    const float buttonSize = 32.0f;
    const float buttonSpacing = 4.0f;
    const float margin = 8.0f;

    // Calculate number of buttons
    int numButtons = m_showMaximizeButton ? 3 : 2;

    // Position buttons at top-right
    float windowWidth = buttonSize * numButtons + buttonSpacing * (numButtons - 1);
    float startX = m_width - margin - windowWidth;
    float startY = margin;

    ImGui::SetNextWindowPos(ImVec2(startX, startY));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, buttonSize));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(buttonSpacing, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));  // Center icon in button

    if (ImGui::Begin("##WindowControls", nullptr, flags)) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();
        float rounding = 6.0f;

        // Helper lambda for centered icon button
        auto IconButton = [&](const char* id, const char* icon, ImVec4 normalColor, ImVec4 hoverColor, ImVec4 activeColor, float xOffset) -> bool {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton(id, ImVec2(buttonSize, buttonSize));
            bool clicked = ImGui::IsItemClicked();
            bool hovered = ImGui::IsItemHovered();
            bool active = ImGui::IsItemActive();

            // Choose color based on state
            ImVec4 color = active ? activeColor : (hovered ? hoverColor : normalColor);
            ImU32 bgColor = ImGui::ColorConvertFloat4ToU32(color);

            // Draw button background
            drawList->AddRectFilled(pos, ImVec2(pos.x + buttonSize, pos.y + buttonSize), bgColor, rounding);

            // Calculate centered text position
            ImVec2 textSize = ImGui::CalcTextSize(icon);
            float textX = pos.x + (buttonSize - textSize.x) * 0.5f + xOffset;
            float textY = pos.y + (buttonSize - textSize.y) * 0.5f;

            // Draw icon
            drawList->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), icon);

            return clicked;
        };

        // Minimize button
        if (IconButton("##min", ICON_FA_WINDOW_MINIMIZE,
            ImVec4(0.3f, 0.3f, 0.35f, 0.6f),
            ImVec4(0.4f, 0.4f, 0.45f, 0.8f),
            ImVec4(0.35f, 0.35f, 0.4f, 0.9f), 0.0f)) {
            ShowWindow(m_hwnd, SW_MINIMIZE);
        }

        // Maximize/Restore button (optional)
        if (m_showMaximizeButton) {
            ImGui::SameLine();

            WINDOWPLACEMENT wp = { sizeof(wp) };
            GetWindowPlacement(m_hwnd, &wp);
            const char* maxIcon = (wp.showCmd == SW_MAXIMIZE) ? ICON_FA_WINDOW_RESTORE : ICON_FA_WINDOW_MAXIMIZE;

            if (IconButton("##max", maxIcon,
                ImVec4(0.3f, 0.3f, 0.35f, 0.6f),
                ImVec4(0.4f, 0.4f, 0.45f, 0.8f),
                ImVec4(0.35f, 0.35f, 0.4f, 0.9f), 0.0f)) {
                if (wp.showCmd == SW_MAXIMIZE) {
                    ShowWindow(m_hwnd, SW_RESTORE);
                } else {
                    ShowWindow(m_hwnd, SW_MAXIMIZE);
                }
            }
        }

        ImGui::SameLine();

        // Close button (red)
        if (IconButton("##close", ICON_FA_XMARK,
            ImVec4(0.8f, 0.2f, 0.2f, 0.6f),
            ImVec4(0.9f, 0.3f, 0.3f, 0.9f),
            ImVec4(0.7f, 0.15f, 0.15f, 1.0f), 0.0f)) {
            m_running = false;
        }
    }
    ImGui::End();

    ImGui::PopStyleVar(4);
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

    // Update login screen (for delay timer)
    if (m_loginScreen) {
        m_loginScreen->Update(deltaTime);
    }

    // Update products screen
    if (m_productsScreen) {
        m_productsScreen->Update(deltaTime);

        // Check if launch was requested
        if (m_productsScreen->ShouldLaunchProduct()) {
            m_productsScreen->ResetLaunch();
            m_state = AppState::GUIMenu;
            // Maximize window for overlay mode
            ShowWindow(m_hwnd, SW_MAXIMIZE);
        }
    }

    // Update HUD overlay
    if (m_hudOverlay) {
        m_hudOverlay->Update(deltaTime);
        m_hudOverlay->SetFPS(1.0f / deltaTime);
    }

    // Update logic - state transitions
    // Only transition from Login to Products if currently in Login state
    if (m_state == AppState::Login && m_loginScreen && m_loginScreen->IsLoggedIn()) {
        // Check if we should skip entry page
        if (m_debugController && m_debugController->skipEntryPage) {
            m_state = AppState::GUIMenu;
            // Maximize window for overlay mode
            ShowWindow(m_hwnd, SW_MAXIMIZE);
        } else {
            m_state = AppState::Products;
        }
    }
}

void Application::Render() {
    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Clear background - black for GUIMenu (overlay compositing), dark gray otherwise
    if (m_state == AppState::GUIMenu) {
        m_dx11->BeginFrame(0.0f, 0.0f, 0.0f, 1.0f);  // Pure black for transparency
    } else {
        m_dx11->BeginFrame(0.06f, 0.06f, 0.09f, 1.0f);
    }

    // Render video background first (only on login screen)
    if (m_state == AppState::Login) {
        RenderVideoBackground();

        // Render window control buttons only on login screen
        // (Products screen has its own controls in account bar)
        RenderWindowControls();
    }

    // Render current screen
    switch (m_state) {
        case AppState::Login:
            if (m_loginScreen) {
                // Render blurred panel background (frosted glass effect)
                const float panelWidth = 480.0f;
                const float panelHeight = 560.0f;
                float panelX = (m_width - panelWidth) * 0.5f;
                float panelY = (m_height - panelHeight) * 0.5f;
                RenderBlurredPanelBackground(panelX, panelY, panelWidth, panelHeight);

                m_loginScreen->Render(m_width, m_height);
            }
            break;

        case AppState::Products:
            if (m_productsScreen) {
                // Set window control callback
                m_productsScreen->SetWindowControlCallback([this](int action) {
                    if (action == 0) {
                        ShowWindow(m_hwnd, SW_MINIMIZE);
                    } else if (action == 1) {
                        m_running = false;
                    }
                });
                m_productsScreen->Render(m_width, m_height);
            }
            break;

        case AppState::GUIMenu:
            // Render HUD overlay (header/footer bars, messages)
            if (m_hudOverlay) {
                m_hudOverlay->Render(m_width, m_height);
            }

            // Render GUI menu
            if (m_guiMenuScreen) {
                m_guiMenuScreen->SetWindowControlCallback([this](int action) {
                    if (action == 1) {
                        // Close goes back to Products
                        m_state = AppState::Products;
                    }
                });
                m_guiMenuScreen->Render(m_width, m_height);
            }
            break;
    }

    // Render debug controller (only on Login screen)
    if (m_state == AppState::Login && m_debugController) {
        m_debugController->Render(m_loginScreen.get());
    }

    // Render ImGui
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    m_dx11->EndFrame();
}

void Application::OnResize(int width, int height) {
    if (width > 0 && height > 0 && m_dx11 && m_dx11->GetDevice()) {
        m_width = width;
        m_height = height;
        m_dx11->Resize(width, height);
        if (m_blurEffect) {
            m_blurEffect->Resize(width, height);
        }
    }
}

void Application::Shutdown() {
    if (m_blurEffect) {
        m_blurEffect->Shutdown();
    }

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
        case WM_NCCALCSIZE:
            // Remove standard window frame
            if (wParam == TRUE) {
                // Return 0 to remove the standard frame
                return 0;
            }
            break;

        case WM_NCHITTEST: {
            // Handle hit testing for resize and drag
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);

            RECT rc;
            GetClientRect(hwnd, &rc);

            const int borderWidth = 6;  // Resize border width
            const int titleHeight = 40; // Draggable title area height

            // Check resize borders
            bool onLeft = pt.x < borderWidth;
            bool onRight = pt.x >= rc.right - borderWidth;
            bool onTop = pt.y < borderWidth;
            bool onBottom = pt.y >= rc.bottom - borderWidth;

            if (onTop && onLeft) return HTTOPLEFT;
            if (onTop && onRight) return HTTOPRIGHT;
            if (onBottom && onLeft) return HTBOTTOMLEFT;
            if (onBottom && onRight) return HTBOTTOMRIGHT;
            if (onLeft) return HTLEFT;
            if (onRight) return HTRIGHT;
            if (onTop) return HTTOP;
            if (onBottom) return HTBOTTOM;

            // Check if in title bar area (top 40px, but not over control buttons)
            if (pt.y < titleHeight && pt.x < rc.right - 120) {
                return HTCAPTION;
            }

            return HTCLIENT;
        }

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
