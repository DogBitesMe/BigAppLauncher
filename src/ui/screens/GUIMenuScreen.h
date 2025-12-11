#pragma once

#include <string>
#include <functional>
#include "../StyleUI.h"

class GUIMenuScreen {
public:
    GUIMenuScreen();
    ~GUIMenuScreen() = default;

    // Render the screen
    void Render(int windowWidth, int windowHeight);

    // For overlay mode (transparent background)
    void RenderOverlay(int windowWidth, int windowHeight);

    // Window controls callback
    using WindowControlCallback = std::function<void(int action)>;
    void SetWindowControlCallback(WindowControlCallback callback) { m_windowControlCallback = callback; }

private:
    // Tab pages
    enum class Tab {
        Visual = 0,
        Items,
        Radar,
        General
    };
    Tab m_currentTab = Tab::Visual;
    int m_tabIndex = 0;

    // Tab content renderers
    void RenderVisualTab(float width, float height);
    void RenderItemsTab(float width, float height);
    void RenderRadarTab(float width, float height);
    void RenderGeneralTab(float width, float height);

    // Sample state for demos
    bool m_checkboxValue1 = true;
    bool m_checkboxValue2 = false;
    bool m_checkboxValue3 = true;
    int m_radioValue = 0;
    float m_sliderFloat = 0.5f;
    int m_sliderInt = 50;
    float m_color[4] = { 0.8f, 0.2f, 0.2f, 1.0f };

    // Visual settings
    bool m_espEnabled = true;
    bool m_boxEsp = true;
    bool m_nameEsp = true;
    bool m_healthBar = true;
    bool m_distanceEsp = false;
    float m_espDistance = 500.0f;

    // Items settings
    bool m_itemEsp = true;
    bool m_weaponEsp = true;
    bool m_vehicleEsp = false;
    int m_itemFilterIndex = 0;

    // Radar settings
    bool m_radarEnabled = true;
    float m_radarSize = 200.0f;
    float m_radarZoom = 1.0f;
    int m_radarStyleIndex = 0;

    // General settings
    bool m_autoAim = false;
    float m_aimFov = 10.0f;
    float m_aimSmooth = 5.0f;

    // Hotkey bindings
    StyleUI::HotkeyBinding m_toggleMenuKey = { 0x2D, false, false, false }; // INSERT
    StyleUI::HotkeyBinding m_toggleEspKey = { 0x70, false, false, false };  // F1
    StyleUI::HotkeyBinding m_toggleAimKey = { 0x71, false, false, false };  // F2

    // Theme selection
    int m_themeIndex = 0;

    // Window controls
    WindowControlCallback m_windowControlCallback;

    // Layout constants
    static constexpr float HEADER_HEIGHT = 45.0f;
    static constexpr float TAB_HEIGHT = 60.0f;
    static constexpr float PADDING = 15.0f;
};
