#pragma once

#include <string>
#include <functional>

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

    // Tab content renderers
    void RenderTabs(float width);
    void RenderVisualTab(float width, float height);
    void RenderItemsTab(float width, float height);
    void RenderRadarTab(float width, float height);
    void RenderGeneralTab(float width, float height);

    // UI Component demos
    void DemoGroupBox(const char* label, float width);
    void DemoSliders();
    void DemoCheckboxes();
    void DemoRadioButtons();
    void DemoColorPicker();
    void DemoButtons();

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

    // Radar settings
    bool m_radarEnabled = true;
    float m_radarSize = 200.0f;
    float m_radarZoom = 1.0f;

    // General settings
    bool m_autoAim = false;
    float m_aimFov = 10.0f;
    float m_aimSmooth = 5.0f;

    // Window controls
    WindowControlCallback m_windowControlCallback;

    // Layout constants
    static constexpr float HEADER_HEIGHT = 45.0f;
    static constexpr float TAB_HEIGHT = 40.0f;
    static constexpr float PADDING = 15.0f;
};
