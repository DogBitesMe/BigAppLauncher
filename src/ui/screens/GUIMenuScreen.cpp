#include "GUIMenuScreen.h"
#include "../Theme.h"
#include "../StyleUI.h"
#include "../IconsFontAwesome6.h"
#include "../../i18n/Localization.h"
#include <imgui.h>

GUIMenuScreen::GUIMenuScreen() {
    // Initialize with default theme
    StyleUI::SetColorScheme(StyleUI::GetDarkBlueScheme());
}

void GUIMenuScreen::Render(int windowWidth, int windowHeight) {
    float width = 480.0f;
    float height = 600.0f;

    // Center the menu
    float x = (windowWidth - width) * 0.5f;
    float y = (windowHeight - height) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    const auto& colors = StyleUI::GetColorScheme();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, colors.Background);
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
        const char* title = ICON_FA_GEAR "  Settings Menu";
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

        // Tab bar using StyleUI
        ImGui::SetCursorPos(ImVec2(PADDING, HEADER_HEIGHT + 8));
        ImGui::PushItemWidth(windowSize.x - PADDING * 2);

        const char* tabIcons[] = { ICON_FA_EYE, ICON_FA_BOX, ICON_FA_SATELLITE_DISH, ICON_FA_GUN, ICON_FA_KEYBOARD, ICON_FA_GEAR };
        const char* tabLabels[] = { "Visual", "Items", "Radar", "Weapon", "Input", "General" };

        m_tabIndex = StyleUI::TabBarLarge("##MainTabs", tabIcons, tabLabels, 6, m_tabIndex);
        m_currentTab = static_cast<Tab>(m_tabIndex);

        ImGui::PopItemWidth();

        // Content area
        float contentY = HEADER_HEIGHT + TAB_HEIGHT + 16;
        float contentHeight = windowSize.y - contentY - PADDING;

        ImGui::SetCursorPos(ImVec2(PADDING, contentY));

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
        ImGui::BeginChild("##Content", ImVec2(windowSize.x - PADDING * 2, contentHeight), false);

        switch (m_currentTab) {
            case Tab::Visual:
                RenderVisualTab(windowSize.x - PADDING * 2, contentHeight);
                break;
            case Tab::Items:
                RenderItemsTab(windowSize.x - PADDING * 2, contentHeight);
                break;
            case Tab::Radar:
                RenderRadarTab(windowSize.x - PADDING * 2, contentHeight);
                break;
            case Tab::Weapon:
                RenderWeaponTab(windowSize.x - PADDING * 2, contentHeight);
                break;
            case Tab::Input:
                RenderInputTab(windowSize.x - PADDING * 2, contentHeight);
                break;
            case Tab::General:
                RenderGeneralTab(windowSize.x - PADDING * 2, contentHeight);
                break;
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void GUIMenuScreen::RenderOverlay(int windowWidth, int windowHeight) {
    // For HUD overlay mode - render transparent elements
}

void GUIMenuScreen::RenderVisualTab(float width, float height) {
    // ESP Settings GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_EYE, "ESP Settings")) {
        StyleUI::ToggleSwitch("Enable ESP", &m_espEnabled);

        if (m_espEnabled) {
            ImGui::Spacing();
            StyleUI::Checkbox("Box ESP", &m_boxEsp);
            StyleUI::Checkbox("Name ESP", &m_nameEsp);
            StyleUI::Checkbox("Health Bar", &m_healthBar);
            StyleUI::Checkbox("Distance", &m_distanceEsp);

            ImGui::Spacing();
            StyleUI::SliderFloat("ESP Distance", &m_espDistance, 100.0f, 1000.0f, "%.0f m");
        }

        StyleUI::EndGroupBox();
    }

    // Color Settings GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_PALETTE, "Color Settings")) {
        StyleUI::ColorEdit4("ESP Color", m_color);

        ImGui::Spacing();

        // Gradient slider demo
        StyleUI::SliderFloatGradient("Opacity", &m_color[3], 0.0f, 1.0f,
            ImVec4(0.2f, 0.2f, 0.25f, 1.0f),
            ImVec4(0.26f, 0.59f, 0.98f, 1.0f),
            "%.0f%%");

        StyleUI::EndGroupBox();
    }
}

void GUIMenuScreen::RenderItemsTab(float width, float height) {
    // Item ESP GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_BOX_OPEN, "Item ESP")) {
        StyleUI::ToggleSwitch("Enable Item ESP", &m_itemEsp);

        if (m_itemEsp) {
            ImGui::Spacing();
            StyleUI::Checkbox("Weapon ESP", &m_weaponEsp);
            StyleUI::Checkbox("Vehicle ESP", &m_vehicleEsp);

            ImGui::Spacing();

            const char* filterItems[] = { "All Items", "Weapons Only", "Ammo Only", "Medical" };
            StyleUI::Combo("Item Filter", &m_itemFilterIndex, filterItems, 4);
        }

        StyleUI::EndGroupBox();
    }

    // Button Demos GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_HAND_POINTER, "Button Styles")) {
        if (StyleUI::Button("Standard Button", ImVec2(width - 30, 0))) {
            // Action
        }

        ImGui::Spacing();

        if (StyleUI::ButtonGradient("Gradient Button", ImVec2(width - 30, 0))) {
            // Action
        }

        ImGui::Spacing();

        if (StyleUI::ButtonOutline("Outline Button", ImVec2(width - 30, 0))) {
            // Action
        }

        ImGui::Spacing();

        if (StyleUI::ButtonIcon(ICON_FA_ROCKET, "Icon Button", ImVec2(width - 30, 0))) {
            // Action
        }

        StyleUI::EndGroupBox();
    }
}

void GUIMenuScreen::RenderRadarTab(float width, float height) {
    // Radar Settings GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_SATELLITE_DISH, "Radar Settings")) {
        StyleUI::ToggleSwitch("Enable Radar", &m_radarEnabled);

        if (m_radarEnabled) {
            ImGui::Spacing();
            StyleUI::SliderFloat("Radar Size", &m_radarSize, 100.0f, 400.0f, "%.0f px");
            StyleUI::SliderFloat("Radar Zoom", &m_radarZoom, 0.5f, 3.0f, "%.1fx");

            ImGui::Spacing();

            const char* radarStyles[] = { "Square", "Circle", "Minimal" };
            StyleUI::Combo("Radar Style", &m_radarStyleIndex, radarStyles, 3);
        }

        StyleUI::EndGroupBox();
    }

    // Checkbox Demos
    if (StyleUI::BeginGroupBoxEx(ICON_FA_SQUARE_CHECK, "Checkbox Styles")) {
        // Text left, checkbox right (our default)
        StyleUI::Checkbox("Modern Checkbox 1", &m_checkboxValue1);
        StyleUI::Checkbox("Modern Checkbox 2", &m_checkboxValue2);

        ImGui::Spacing();
        StyleUI::SeparatorText("Classic Style");

        // Classic checkbox left, text right
        StyleUI::CheckboxClassic("Classic Checkbox", &m_checkboxValue3);

        StyleUI::EndGroupBox();
    }
}

void GUIMenuScreen::RenderWeaponTab(float width, float height) {
    // SubTab Style Selector
    if (StyleUI::BeginGroupBoxEx(ICON_FA_SLIDERS, "SubTab Style")) {
        const char* styleLabels[] = { "Underline", "Pill" };
        m_weaponSubTabStyle = StyleUI::RadioButtonGroup("##SubTabStyleSelector", styleLabels, 2, m_weaponSubTabStyle);
        StyleUI::EndGroupBox();
    }

    // SubTab Navigation
    const char* subTabLabels[] = { "Rifle", "SMG", "Sniper", "Pistol" };
    const char* subTabIcons[] = { ICON_FA_CROSSHAIRS, ICON_FA_BOLT, ICON_FA_BULLSEYE, ICON_FA_HAND };
    StyleUI::SubTabStyle style = m_weaponSubTabStyle == 0 ? StyleUI::SubTabStyle::Underline : StyleUI::SubTabStyle::Pill;
    m_weaponSubTabIndex = StyleUI::SubTabIcon("##WeaponSubTabs", subTabIcons, subTabLabels, 4, m_weaponSubTabIndex, style);

    ImGui::Spacing();

    // Content based on selected sub-tab
    switch (m_weaponSubTabIndex) {
        case 0: // Rifle
            if (StyleUI::BeginGroupBoxEx(ICON_FA_CROSSHAIRS, "Rifle Settings")) {
                StyleUI::ToggleSwitch("No Recoil", &m_rifleNoRecoil);
                StyleUI::Checkbox("No Spread", &m_rifleNoSpread);
                ImGui::Spacing();
                StyleUI::SliderFloat("Damage Multiplier", &m_rifleDamageMultiplier, 1.0f, 5.0f, "%.1fx");
                StyleUI::EndGroupBox();
            }
            break;

        case 1: // SMG
            if (StyleUI::BeginGroupBoxEx(ICON_FA_BOLT, "SMG Settings")) {
                StyleUI::ToggleSwitch("No Recoil", &m_smgNoRecoil);
                StyleUI::Checkbox("Rapid Fire", &m_smgRapidFire);
                ImGui::Spacing();
                StyleUI::SliderFloat("Fire Rate Multiplier", &m_smgFireRateMultiplier, 1.0f, 3.0f, "%.1fx");
                StyleUI::EndGroupBox();
            }
            break;

        case 2: // Sniper
            if (StyleUI::BeginGroupBoxEx(ICON_FA_BULLSEYE, "Sniper Settings")) {
                StyleUI::ToggleSwitch("No Sway", &m_sniperNoSway);
                StyleUI::Checkbox("Instant Aim", &m_sniperInstantAim);
                ImGui::Spacing();
                StyleUI::SliderFloat("Zoom Multiplier", &m_sniperZoomMultiplier, 1.0f, 10.0f, "%.1fx");
                StyleUI::EndGroupBox();
            }
            break;

        case 3: // Pistol
            if (StyleUI::BeginGroupBoxEx(ICON_FA_HAND, "Pistol Settings")) {
                StyleUI::ToggleSwitch("No Recoil", &m_pistolNoRecoil);
                StyleUI::Checkbox("Auto Fire", &m_pistolAutoFire);
                ImGui::Spacing();

                const char* fireModes[] = { "Semi-Auto", "Burst", "Full Auto" };
                StyleUI::Combo("Fire Mode", &m_pistolFireModeIndex, fireModes, 3);

                ImGui::Spacing();
                StyleUI::SeparatorText("RadioButton Demo");

                // Demo classic radio buttons
                StyleUI::RadioButtonClassic("Semi-Auto Mode", &m_pistolFireModeIndex, 0);
                StyleUI::RadioButtonClassic("Burst Mode", &m_pistolFireModeIndex, 1);
                StyleUI::RadioButtonClassic("Full Auto Mode", &m_pistolFireModeIndex, 2);

                StyleUI::EndGroupBox();
            }
            break;
    }
}

void GUIMenuScreen::RenderInputTab(float width, float height) {
    // TextInput Demo GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_FONT, "TextInput Demos")) {
        // Label Above (default)
        StyleUI::TextInput("Label Above", m_textInputAbove, sizeof(m_textInputAbove),
                          StyleUI::TextInputLabelMode::Above, "Enter text here...");

        ImGui::Spacing();

        // Label Below
        StyleUI::TextInput("Label Below", m_textInputBelow, sizeof(m_textInputBelow),
                          StyleUI::TextInputLabelMode::Below, "Enter text here...");

        ImGui::Spacing();

        // Placeholder Mode (label as placeholder)
        StyleUI::TextInput("Enter your name...", m_textInputPlaceholder, sizeof(m_textInputPlaceholder),
                          StyleUI::TextInputLabelMode::Placeholder);

        ImGui::Spacing();

        // Hidden label with custom placeholder
        StyleUI::TextInput("##HiddenLabel", m_textInputHidden, sizeof(m_textInputHidden),
                          StyleUI::TextInputLabelMode::Hidden, "Search...");

        StyleUI::EndGroupBox();
    }

    // Password Demo GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_LOCK, "Password Input")) {
        StyleUI::PasswordInput("Password", m_passwordInput, sizeof(m_passwordInput),
                              StyleUI::TextInputLabelMode::Above, "Enter password...");

        StyleUI::EndGroupBox();
    }
}

void GUIMenuScreen::RenderGeneralTab(float width, float height) {
    // Aim Assist GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_CROSSHAIRS, "Aim Assist")) {
        StyleUI::ToggleSwitch("Enable Aim Assist", &m_autoAim);

        if (m_autoAim) {
            ImGui::Spacing();
            StyleUI::SliderFloat("FOV", &m_aimFov, 1.0f, 30.0f, "%.1f");
            StyleUI::SliderFloat("Smoothness", &m_aimSmooth, 1.0f, 20.0f, "%.1f");
        }

        StyleUI::EndGroupBox();
    }

    // Hotkey Bindings GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_KEYBOARD, "Hotkey Bindings")) {
        StyleUI::HotkeyInput("Toggle Menu", &m_toggleMenuKey);
        StyleUI::HotkeyInput("Toggle ESP", &m_toggleEspKey);
        StyleUI::HotkeyInput("Toggle Aim", &m_toggleAimKey);

        StyleUI::EndGroupBox();
    }

    // Theme Selection GroupBox
    if (StyleUI::BeginGroupBoxEx(ICON_FA_BRUSH, "Theme")) {
        const char* themes[] = { "Dark Blue", "Dark Purple", "Dark Green", "Dark Red" };
        int prevTheme = m_themeIndex;
        StyleUI::Combo("Color Scheme", &m_themeIndex, themes, 4);

        if (m_themeIndex != prevTheme) {
            switch (m_themeIndex) {
                case 0: StyleUI::SetColorScheme(StyleUI::GetDarkBlueScheme()); break;
                case 1: StyleUI::SetColorScheme(StyleUI::GetDarkPurpleScheme()); break;
                case 2: StyleUI::SetColorScheme(StyleUI::GetDarkGreenScheme()); break;
                case 3: StyleUI::SetColorScheme(StyleUI::GetDarkRedScheme()); break;
            }
        }

        ImGui::Spacing();

        // Demo slider values
        StyleUI::SeparatorText("Slider Demo");
        StyleUI::SliderFloat("Float Value", &m_sliderFloat, 0.0f, 1.0f, "%.2f");
        StyleUI::SliderInt("Integer Value", &m_sliderInt, 0, 100, "%d");

        StyleUI::EndGroupBox();
    }
}
