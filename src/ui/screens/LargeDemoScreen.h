#pragma once

#include <string>
#include <functional>
#include "../StyleUI.h"

// Large demo screen with NEXO-style layout
// Designed for 1920x1080, takes ~80% height and ~70-80% width
class LargeDemoScreen {
public:
    LargeDemoScreen();
    ~LargeDemoScreen() = default;

    // Render the screen
    void Render(int windowWidth, int windowHeight);

    // Callback to switch to component demo (old GUIMenuScreen)
    using SwitchCallback = std::function<void()>;
    void SetSwitchToComponentDemoCallback(SwitchCallback callback) { m_switchCallback = callback; }

    // Window controls callback
    using WindowControlCallback = std::function<void(int action)>;
    void SetWindowControlCallback(WindowControlCallback callback) { m_windowControlCallback = callback; }

private:
    // Main tabs (Pill style)
    enum class MainTab {
        Aimbot = 0,
        Visual,
        Misc,
        Settings
    };
    MainTab m_currentMainTab = MainTab::Aimbot;
    int m_mainTabIndex = 0;

    // Sub tabs for each main tab (Button style)
    int m_aimbotSubTab = 0;
    int m_visualSubTab = 0;
    int m_miscSubTab = 0;
    int m_settingsSubTab = 0;

    // Render sections
    void RenderHeader(float width);
    void RenderFooter(float width, float y);
    void RenderContent(float width, float height);

    // Tab content renderers
    void RenderAimbotTab(float width, float height);
    void RenderVisualTab(float width, float height);
    void RenderMiscTab(float width, float height);
    void RenderSettingsTab(float width, float height);

    // Aimbot settings
    bool m_aimbotEnabled = true;
    bool m_aimbotSilent = false;
    bool m_aimbotVisibleOnly = true;
    float m_aimbotFov = 15.0f;
    float m_aimbotSmooth = 5.0f;
    int m_aimbotBoneIndex = 0;  // 0=Head, 1=Neck, 2=Chest
    int m_aimbotKeyIndex = 0;
    int m_targetPriority = 0;   // OptionRow demo: 0=Distance, 1=Crosshair, 2=Health, 3=Visibility
    StyleUI::HotkeyBinding m_aimbotKey = { 0x02, false, false, false }; // Right mouse
    float m_aimSpeed = 5.0f;
    float m_aimAcceleration = 2.0f;
    bool m_holdToAim = true;

    // Triggerbot settings
    bool m_triggerbotEnabled = false;
    float m_triggerbotDelay = 50.0f;
    bool m_triggerbotOnKey = true;

    // Visual settings
    bool m_espEnabled = true;
    bool m_espBox = true;
    bool m_espSkeleton = true;
    bool m_espName = true;
    bool m_espHealth = true;
    bool m_espDistance = true;
    bool m_espSnaplines = false;
    float m_espMaxDistance = 500.0f;
    float m_espBoxColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float m_espNameColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Chams settings
    bool m_chamsEnabled = false;
    bool m_chamsVisibleOnly = false;
    int m_chamsMaterialIndex = 0;
    float m_chamsVisibleColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    float m_chamsHiddenColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

    // Misc settings
    bool m_bunnyHop = false;
    bool m_autoStrafe = false;
    bool m_noFlash = true;
    bool m_noSmoke = false;
    float m_flashAlpha = 0.0f;
    bool m_radarEnabled = true;
    float m_radarSize = 200.0f;
    float m_radarZoom = 1.0f;

    // Settings
    int m_themeIndex = 0;
    std::string m_currentLanguage = "en";
    bool m_saveOnExit = true;
    bool m_loadOnStart = true;

    // Callbacks
    SwitchCallback m_switchCallback;
    WindowControlCallback m_windowControlCallback;

    // Layout constants
    static constexpr float HEADER_HEIGHT = 50.0f;
    static constexpr float FOOTER_HEIGHT = 40.0f;
    static constexpr float PADDING = 20.0f;
    static constexpr float LEFT_PANEL_WIDTH = 280.0f;
    static constexpr float COLUMN_SPACING = 16.0f;  // Space between Lv1 boxes

    // Debug
    static constexpr bool DEBUG_SHOW_BORDERS = false;
};
