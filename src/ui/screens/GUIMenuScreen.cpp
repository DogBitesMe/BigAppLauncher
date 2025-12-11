#include "GUIMenuScreen.h"
#include "../Theme.h"
#include "../Widgets.h"
#include "../IconsFontAwesome6.h"
#include "../../i18n/Localization.h"
#include <imgui.h>

GUIMenuScreen::GUIMenuScreen() {
}

void GUIMenuScreen::Render(int windowWidth, int windowHeight) {
    float width = 450.0f;
    float height = 550.0f;

    // Center the menu
    float x = (windowWidth - width) * 0.5f;
    float y = (windowHeight - height) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.12f, 0.95f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (ImGui::Begin("##GUIMenu", nullptr, flags)) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        // Header
        drawList->AddRectFilled(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + HEADER_HEIGHT),
            IM_COL32(15, 15, 22, 255),
            12.0f,
            ImDrawFlags_RoundCornersTop
        );

        // Title
        const char* title = "Settings Menu";
        ImVec2 titleSize = ImGui::CalcTextSize(title);
        drawList->AddText(
            ImVec2(windowPos.x + 15, windowPos.y + (HEADER_HEIGHT - titleSize.y) * 0.5f),
            IM_COL32(220, 220, 220, 255),
            title
        );

        // Close button
        float closeSize = 28.0f;
        ImVec2 closePos(windowPos.x + windowSize.x - closeSize - 10, windowPos.y + (HEADER_HEIGHT - closeSize) * 0.5f);
        bool closeHovered = ImGui::IsMouseHoveringRect(closePos, ImVec2(closePos.x + closeSize, closePos.y + closeSize));

        drawList->AddRectFilled(
            closePos,
            ImVec2(closePos.x + closeSize, closePos.y + closeSize),
            closeHovered ? IM_COL32(200, 60, 60, 255) : IM_COL32(60, 60, 70, 200),
            6.0f
        );

        ImVec2 closeIconSize = ImGui::CalcTextSize(ICON_FA_XMARK);
        drawList->AddText(
            ImVec2(closePos.x + (closeSize - closeIconSize.x) * 0.5f, closePos.y + (closeSize - closeIconSize.y) * 0.5f),
            IM_COL32(255, 255, 255, 255),
            ICON_FA_XMARK
        );

        if (closeHovered && ImGui::IsMouseClicked(0) && m_windowControlCallback) {
            m_windowControlCallback(1);
        }

        // Tabs
        ImGui::SetCursorPos(ImVec2(0, HEADER_HEIGHT));
        RenderTabs(windowSize.x);

        // Content
        float contentY = HEADER_HEIGHT + TAB_HEIGHT;
        float contentHeight = windowSize.y - contentY;

        ImGui::SetCursorPos(ImVec2(PADDING, contentY + PADDING));

        ImGui::BeginChild("##Content", ImVec2(windowSize.x - PADDING * 2, contentHeight - PADDING * 2), false);

        switch (m_currentTab) {
            case Tab::Visual:
                RenderVisualTab(windowSize.x - PADDING * 2, contentHeight - PADDING * 2);
                break;
            case Tab::Items:
                RenderItemsTab(windowSize.x - PADDING * 2, contentHeight - PADDING * 2);
                break;
            case Tab::Radar:
                RenderRadarTab(windowSize.x - PADDING * 2, contentHeight - PADDING * 2);
                break;
            case Tab::General:
                RenderGeneralTab(windowSize.x - PADDING * 2, contentHeight - PADDING * 2);
                break;
        }

        ImGui::EndChild();
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void GUIMenuScreen::RenderOverlay(int windowWidth, int windowHeight) {
    // For HUD overlay mode - render transparent elements
    // This will be implemented in HUDOverlay class
}

void GUIMenuScreen::RenderTabs(float width) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    float tabY = windowPos.y + HEADER_HEIGHT;

    // Tab background
    drawList->AddRectFilled(
        ImVec2(windowPos.x, tabY),
        ImVec2(windowPos.x + width, tabY + TAB_HEIGHT),
        IM_COL32(20, 20, 28, 255)
    );

    struct TabInfo {
        const char* icon;
        const char* label;
        Tab tab;
    };

    TabInfo tabs[] = {
        { ICON_FA_EYE, "Visual", Tab::Visual },
        { ICON_FA_BOX, "Items", Tab::Items },
        { ICON_FA_SATELLITE_DISH, "Radar", Tab::Radar },
        { ICON_FA_GEAR, "General", Tab::General },
    };

    float tabWidth = width / 4.0f;

    for (int i = 0; i < 4; i++) {
        ImVec2 tabPos(windowPos.x + i * tabWidth, tabY);
        bool isActive = (m_currentTab == tabs[i].tab);
        bool isHovered = ImGui::IsMouseHoveringRect(tabPos, ImVec2(tabPos.x + tabWidth, tabPos.y + TAB_HEIGHT));

        // Tab background
        if (isActive) {
            drawList->AddRectFilled(
                tabPos,
                ImVec2(tabPos.x + tabWidth, tabPos.y + TAB_HEIGHT),
                IM_COL32(66, 150, 255, 40)
            );

            // Active indicator
            drawList->AddRectFilled(
                ImVec2(tabPos.x, tabPos.y + TAB_HEIGHT - 3),
                ImVec2(tabPos.x + tabWidth, tabPos.y + TAB_HEIGHT),
                IM_COL32(66, 150, 255, 255)
            );
        } else if (isHovered) {
            drawList->AddRectFilled(
                tabPos,
                ImVec2(tabPos.x + tabWidth, tabPos.y + TAB_HEIGHT),
                IM_COL32(255, 255, 255, 15)
            );
        }

        // Icon and label
        char tabText[64];
        snprintf(tabText, sizeof(tabText), "%s %s", tabs[i].icon, tabs[i].label);
        ImVec2 textSize = ImGui::CalcTextSize(tabText);

        ImU32 textColor = isActive ? IM_COL32(66, 150, 255, 255) : IM_COL32(180, 180, 180, 255);
        drawList->AddText(
            ImVec2(tabPos.x + (tabWidth - textSize.x) * 0.5f, tabPos.y + (TAB_HEIGHT - textSize.y) * 0.5f),
            textColor,
            tabText
        );

        // Handle click
        if (isHovered && ImGui::IsMouseClicked(0)) {
            m_currentTab = tabs[i].tab;
        }
    }
}

void GUIMenuScreen::DemoGroupBox(const char* label, float width) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.14f, 0.6f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);

    ImGui::BeginChild(label, ImVec2(width, 0), ImGuiChildFlags_AutoResizeY);

    // Header
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
}

void GUIMenuScreen::RenderVisualTab(float width, float height) {
    // ESP Section
    DemoGroupBox("ESP Settings", width - 10);

    ImGui::Checkbox("Enable ESP", &m_espEnabled);

    if (m_espEnabled) {
        ImGui::Indent(15);
        ImGui::Checkbox("Box ESP", &m_boxEsp);
        ImGui::Checkbox("Name ESP", &m_nameEsp);
        ImGui::Checkbox("Health Bar", &m_healthBar);
        ImGui::Checkbox("Distance", &m_distanceEsp);
        ImGui::Unindent(15);

        ImGui::Spacing();
        ImGui::Text("ESP Distance:");
        ImGui::SliderFloat("##ESPDist", &m_espDistance, 100.0f, 1000.0f, "%.0f m");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Color Settings
    DemoGroupBox("Color Settings", width - 10);

    ImGui::Text("ESP Color:");
    ImGui::ColorEdit4("##ESPColor", m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void GUIMenuScreen::RenderItemsTab(float width, float height) {
    DemoGroupBox("Item ESP", width - 10);

    ImGui::Checkbox("Enable Item ESP", &m_itemEsp);
    ImGui::Checkbox("Weapon ESP", &m_weaponEsp);
    ImGui::Checkbox("Vehicle ESP", &m_vehicleEsp);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Demo various UI components
    ImGui::Text("UI Component Demo:");
    ImGui::Spacing();

    // Buttons
    if (ImGui::Button("Standard Button", ImVec2(150, 30))) {
        // Action
    }

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    if (ImGui::Button("Green Button", ImVec2(150, 30))) {
        // Action
    }
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Gradient button
    if (Widgets::GradientButton("Gradient Button", ImVec2(width - 30, 35))) {
        // Action
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void GUIMenuScreen::RenderRadarTab(float width, float height) {
    DemoGroupBox("Radar Settings", width - 10);

    ImGui::Checkbox("Enable Radar", &m_radarEnabled);

    if (m_radarEnabled) {
        ImGui::Spacing();
        ImGui::Text("Radar Size:");
        ImGui::SliderFloat("##RadarSize", &m_radarSize, 100.0f, 400.0f, "%.0f px");

        ImGui::Text("Radar Zoom:");
        ImGui::SliderFloat("##RadarZoom", &m_radarZoom, 0.5f, 3.0f, "%.1fx");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Radio button demo
    DemoGroupBox("Radio Button Demo", width - 10);

    ImGui::RadioButton("Option 1", &m_radioValue, 0);
    ImGui::RadioButton("Option 2", &m_radioValue, 1);
    ImGui::RadioButton("Option 3", &m_radioValue, 2);

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void GUIMenuScreen::RenderGeneralTab(float width, float height) {
    DemoGroupBox("Aim Assist", width - 10);

    ImGui::Checkbox("Enable Aim Assist", &m_autoAim);

    if (m_autoAim) {
        ImGui::Spacing();
        ImGui::Text("FOV:");
        ImGui::SliderFloat("##AimFOV", &m_aimFov, 1.0f, 30.0f, "%.1f");

        ImGui::Text("Smoothness:");
        ImGui::SliderFloat("##AimSmooth", &m_aimSmooth, 1.0f, 20.0f, "%.1f");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Slider demo
    DemoGroupBox("Slider Demo", width - 10);

    ImGui::Text("Float Slider:");
    ImGui::SliderFloat("##FloatSlider", &m_sliderFloat, 0.0f, 1.0f, "%.2f");

    ImGui::Text("Integer Slider:");
    ImGui::SliderInt("##IntSlider", &m_sliderInt, 0, 100, "%d");

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Checkbox demo
    DemoGroupBox("Checkbox Demo", width - 10);

    ImGui::Checkbox("Checkbox 1", &m_checkboxValue1);
    ImGui::SameLine();
    ImGui::Checkbox("Checkbox 2", &m_checkboxValue2);
    ImGui::SameLine();
    ImGui::Checkbox("Checkbox 3", &m_checkboxValue3);

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}
