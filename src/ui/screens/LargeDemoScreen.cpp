#include "LargeDemoScreen.h"
#include "../Theme.h"
#include "../StyleUI.h"
#include "../IconsFontAwesome6.h"
#include "../../i18n/Localization.h"
#include <imgui.h>

// Helper function to convert ImVec4 to ImU32
static ImU32 ColorToU32(const ImVec4& col) {
    return IM_COL32(
        (int)(col.x * 255.0f),
        (int)(col.y * 255.0f),
        (int)(col.z * 255.0f),
        (int)(col.w * 255.0f)
    );
}

LargeDemoScreen::LargeDemoScreen() {
    StyleUI::SetColorScheme(StyleUI::GetDarkBlueScheme());
    m_currentLanguage = i18n::GetLanguage();
}

void LargeDemoScreen::Render(int windowWidth, int windowHeight) {
    // Calculate menu size (80% height, 75% width for 1920x1080)
    float width = windowWidth * 0.75f;
    float height = windowHeight * 0.80f;

    // Clamp to reasonable bounds
    if (width < 1000.0f) width = 1000.0f;
    if (width > 1500.0f) width = 1500.0f;
    if (height < 600.0f) height = 600.0f;
    if (height > 900.0f) height = 900.0f;

    // Center the menu
    float x = (windowWidth - width) * 0.5f;
    float y = (windowHeight - height) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(x, y));
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    // Dark semi-transparent background
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(18.0f/255.0f, 20.0f/255.0f, 28.0f/255.0f, 0.95f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (ImGui::Begin("##LargeDemoMenu", nullptr, flags)) {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        // Render header
        RenderHeader(windowSize.x);

        // Render content area
        float contentY = HEADER_HEIGHT;
        float contentHeight = windowSize.y - HEADER_HEIGHT - FOOTER_HEIGHT;
        RenderContent(windowSize.x, contentHeight);

        // Render footer
        RenderFooter(windowSize.x, windowSize.y - FOOTER_HEIGHT);
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void LargeDemoScreen::RenderHeader(float width) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    const auto& colors = StyleUI::GetColorScheme();

    // Header background
    drawList->AddRectFilled(
        windowPos,
        ImVec2(windowPos.x + width, windowPos.y + HEADER_HEIGHT),
        IM_COL32(15, 18, 25, 255),
        12.0f,
        ImDrawFlags_RoundCornersTop
    );

    // Logo/Brand area (left side)
    float logoX = windowPos.x + PADDING;
    float logoY = windowPos.y + (HEADER_HEIGHT - 24) * 0.5f;

    // Icon
    drawList->AddText(ImVec2(logoX, logoY), ColorToU32(colors.Primary), ICON_FA_CROSSHAIRS);

    // Brand name
    const char* brandName = "DEMO MENU";
    ImVec2 brandSize = ImGui::CalcTextSize(brandName);
    drawList->AddText(ImVec2(logoX + 30, logoY), IM_COL32(255, 255, 255, 255), brandName);

    // Separator
    drawList->AddText(ImVec2(logoX + 40 + brandSize.x, logoY), IM_COL32(100, 100, 100, 255), "|");

    // Game name
    const char* gameName = "Game Name";
    drawList->AddText(ImVec2(logoX + 60 + brandSize.x, logoY), IM_COL32(150, 150, 150, 255), gameName);

    // Main tabs (Pill style) - centered
    ImGui::SetCursorPos(ImVec2(width * 0.30f, 10));

    const char* mainTabs[] = { "Aimbot", "Visual", "Misc", "Settings" };
    m_mainTabIndex = StyleUI::TabBarPill("##MainTabs", mainTabs, 4, m_mainTabIndex);
    m_currentMainTab = static_cast<MainTab>(m_mainTabIndex);

    // Right side buttons
    float btnY = windowPos.y + (HEADER_HEIGHT - 28) * 0.5f;
    float btnX = windowPos.x + width - PADDING;

    // Close button
    float btnSize = 28.0f;
    btnX -= btnSize;
    ImVec2 closePos(btnX, btnY);
    bool closeHovered = ImGui::IsMouseHoveringRect(closePos, ImVec2(closePos.x + btnSize, closePos.y + btnSize));

    drawList->AddRectFilled(
        closePos,
        ImVec2(closePos.x + btnSize, closePos.y + btnSize),
        closeHovered ? IM_COL32(200, 60, 60, 255) : IM_COL32(60, 60, 70, 180),
        6.0f
    );

    ImVec2 closeIconSize = ImGui::CalcTextSize(ICON_FA_XMARK);
    drawList->AddText(
        ImVec2(closePos.x + (btnSize - closeIconSize.x) * 0.5f, closePos.y + (btnSize - closeIconSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        ICON_FA_XMARK
    );

    if (closeHovered && ImGui::IsMouseClicked(0) && m_windowControlCallback) {
        m_windowControlCallback(1);
    }

    // Minimize button
    btnX -= btnSize + 8;
    ImVec2 minPos(btnX, btnY);
    bool minHovered = ImGui::IsMouseHoveringRect(minPos, ImVec2(minPos.x + btnSize, minPos.y + btnSize));

    drawList->AddRectFilled(
        minPos,
        ImVec2(minPos.x + btnSize, minPos.y + btnSize),
        minHovered ? IM_COL32(80, 80, 90, 255) : IM_COL32(60, 60, 70, 180),
        6.0f
    );

    ImVec2 minIconSize = ImGui::CalcTextSize(ICON_FA_MINUS);
    drawList->AddText(
        ImVec2(minPos.x + (btnSize - minIconSize.x) * 0.5f, minPos.y + (btnSize - minIconSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        ICON_FA_MINUS
    );

    if (minHovered && ImGui::IsMouseClicked(0) && m_windowControlCallback) {
        m_windowControlCallback(2);
    }

    // Component Demo button (switch to old menu)
    btnX -= 120;
    ImVec2 demoPos(btnX, btnY);
    ImVec2 demoBtnSize(110, btnSize);
    bool demoHovered = ImGui::IsMouseHoveringRect(demoPos, ImVec2(demoPos.x + demoBtnSize.x, demoPos.y + demoBtnSize.y));

    drawList->AddRectFilled(
        demoPos,
        ImVec2(demoPos.x + demoBtnSize.x, demoPos.y + demoBtnSize.y),
        demoHovered ? IM_COL32(70, 70, 80, 255) : IM_COL32(50, 50, 60, 180),
        6.0f
    );

    const char* demoText = ICON_FA_CUBES " Widgets";
    ImVec2 demoTextSize = ImGui::CalcTextSize(demoText);
    drawList->AddText(
        ImVec2(demoPos.x + (demoBtnSize.x - demoTextSize.x) * 0.5f, demoPos.y + (demoBtnSize.y - demoTextSize.y) * 0.5f),
        IM_COL32(200, 200, 200, 255),
        demoText
    );

    if (demoHovered && ImGui::IsMouseClicked(0) && m_switchCallback) {
        m_switchCallback();
    }
}

void LargeDemoScreen::RenderFooter(float width, float y) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    const auto& colors = StyleUI::GetColorScheme();

    float footerY = windowPos.y + y;

    // Footer background
    drawList->AddRectFilled(
        ImVec2(windowPos.x, footerY),
        ImVec2(windowPos.x + width, footerY + FOOTER_HEIGHT),
        IM_COL32(15, 18, 25, 255),
        12.0f,
        ImDrawFlags_RoundCornersBottom
    );

    // Version info (left)
    const char* version = "Version: 1.0.0";
    float textY = footerY + (FOOTER_HEIGHT - ImGui::GetFontSize()) * 0.5f;
    drawList->AddText(ImVec2(windowPos.x + PADDING, textY), IM_COL32(120, 120, 120, 255), version);

    // Language selector (center)
    ImGui::SetCursorPos(ImVec2(width * 0.5f - 60, y + 8));
    ImGui::PushItemWidth(120);
    if (StyleUI::LanguageCombo("##FooterLang", m_currentLanguage, 120.0f)) {
        i18n::SetLanguage(m_currentLanguage);
    }
    ImGui::PopItemWidth();

    // Expiry info (right side, before exit)
    const char* expiry = "Expires: 2025-12-31";
    ImVec2 expirySize = ImGui::CalcTextSize(expiry);
    drawList->AddText(ImVec2(windowPos.x + width - PADDING - 80 - expirySize.x, textY), IM_COL32(120, 120, 120, 255), expiry);

    // Exit button (far right)
    float exitBtnWidth = 60.0f;
    float exitBtnX = windowPos.x + width - PADDING - exitBtnWidth;
    float exitBtnY = footerY + 6;
    float exitBtnH = FOOTER_HEIGHT - 12;

    ImVec2 exitPos(exitBtnX, exitBtnY);
    bool exitHovered = ImGui::IsMouseHoveringRect(exitPos, ImVec2(exitPos.x + exitBtnWidth, exitPos.y + exitBtnH));

    drawList->AddRectFilled(
        exitPos,
        ImVec2(exitPos.x + exitBtnWidth, exitPos.y + exitBtnH),
        exitHovered ? IM_COL32(180, 50, 50, 255) : IM_COL32(100, 40, 40, 200),
        4.0f
    );

    const char* exitText = "Exit";
    ImVec2 exitTextSize = ImGui::CalcTextSize(exitText);
    drawList->AddText(
        ImVec2(exitPos.x + (exitBtnWidth - exitTextSize.x) * 0.5f, exitPos.y + (exitBtnH - exitTextSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        exitText
    );

    if (exitHovered && ImGui::IsMouseClicked(0) && m_windowControlCallback) {
        m_windowControlCallback(1); // Same as close
    }
}

void LargeDemoScreen::RenderContent(float width, float height) {
    ImGui::SetCursorPos(ImVec2(PADDING, HEADER_HEIGHT + 10));

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::BeginChild("##Content", ImVec2(width - PADDING * 2, height - 20), false);

    switch (m_currentMainTab) {
        case MainTab::Aimbot:
            RenderAimbotTab(width - PADDING * 2, height - 20);
            break;
        case MainTab::Visual:
            RenderVisualTab(width - PADDING * 2, height - 20);
            break;
        case MainTab::Misc:
            RenderMiscTab(width - PADDING * 2, height - 20);
            break;
        case MainTab::Settings:
            RenderSettingsTab(width - PADDING * 2, height - 20);
            break;
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void LargeDemoScreen::RenderAimbotTab(float width, float height) {
    const auto& colors = StyleUI::GetColorScheme();

    // Two-column layout
    float leftWidth = LEFT_PANEL_WIDTH;
    float rightWidth = width - leftWidth - PADDING;

    // Left column
    ImGui::BeginChild("##AimbotLeft", ImVec2(leftWidth, 0), false);

    // Aimbot Main GroupBox (Flat style)
    if (StyleUI::BeginGroupBoxFlat("Aimbot")) {
        StyleUI::ToggleSwitch("Enable Aimbot", &m_aimbotEnabled);

        if (m_aimbotEnabled) {
            ImGui::Spacing();
            StyleUI::Checkbox("Silent Aim", &m_aimbotSilent);
            StyleUI::Checkbox("Visible Only", &m_aimbotVisibleOnly);

            ImGui::Spacing();
            StyleUI::SliderFloat("FOV", &m_aimbotFov, 1.0f, 90.0f, "%.1f");
            StyleUI::SliderFloat("Smoothness", &m_aimbotSmooth, 1.0f, 20.0f, "%.1f");

            ImGui::Spacing();
            const char* bones[] = { "Head", "Neck", "Chest" };
            StyleUI::Combo("Target Bone", &m_aimbotBoneIndex, bones, 3);
        }

        StyleUI::EndGroupBoxFlat();
    }

    ImGui::Spacing();

    // Triggerbot GroupBox
    if (StyleUI::BeginGroupBoxFlat("Triggerbot")) {
        StyleUI::ToggleSwitch("Enable Triggerbot", &m_triggerbotEnabled);

        if (m_triggerbotEnabled) {
            ImGui::Spacing();
            StyleUI::SliderFloat("Delay (ms)", &m_triggerbotDelay, 0.0f, 200.0f, "%.0f");
            StyleUI::Checkbox("On Key Only", &m_triggerbotOnKey);
        }

        StyleUI::EndGroupBoxFlat();
    }

    ImGui::EndChild();

    // Right column
    ImGui::SameLine();
    ImGui::BeginChild("##AimbotRight", ImVec2(rightWidth, 0), false);

    // Sub-tabs for right panel
    const char* subTabs[] = { "General", "Prediction", "Silent" };
    m_aimbotSubTab = StyleUI::TabBarButton("##AimbotSubTabs", subTabs, 3, m_aimbotSubTab);

    ImGui::Spacing();
    ImGui::Spacing();

    // Main content area with nested GroupBoxes
    if (StyleUI::BeginGroupBoxFlat("Configuration")) {

        // Nested sections
        if (StyleUI::BeginGroupBoxNested("Target Settings")) {
            StyleUI::Checkbox("Auto Switch Target", &m_aimbotVisibleOnly);
            StyleUI::SliderFloat("Switch Delay", &m_triggerbotDelay, 0.0f, 500.0f, "%.0f ms");
            StyleUI::EndGroupBoxNested();
        }

        ImGui::Spacing();

        if (StyleUI::BeginGroupBoxNested("Keybind")) {
            StyleUI::HotkeyInput("Aimbot Key", &m_aimbotKey);
            StyleUI::EndGroupBoxNested();
        }

        StyleUI::EndGroupBoxFlat();
    }

    ImGui::EndChild();
}

void LargeDemoScreen::RenderVisualTab(float width, float height) {
    float leftWidth = LEFT_PANEL_WIDTH;
    float rightWidth = width - leftWidth - PADDING;

    // Left column
    ImGui::BeginChild("##VisualLeft", ImVec2(leftWidth, 0), false);

    if (StyleUI::BeginGroupBoxFlat("ESP")) {
        StyleUI::ToggleSwitch("Enable ESP", &m_espEnabled);

        if (m_espEnabled) {
            ImGui::Spacing();
            StyleUI::Checkbox("Box", &m_espBox);
            StyleUI::Checkbox("Skeleton", &m_espSkeleton);
            StyleUI::Checkbox("Name", &m_espName);
            StyleUI::Checkbox("Health", &m_espHealth);
            StyleUI::Checkbox("Distance", &m_espDistance);
            StyleUI::Checkbox("Snaplines", &m_espSnaplines);

            ImGui::Spacing();
            StyleUI::SliderFloat("Max Distance", &m_espMaxDistance, 100.0f, 1000.0f, "%.0f m");
        }

        StyleUI::EndGroupBoxFlat();
    }

    ImGui::EndChild();

    // Right column
    ImGui::SameLine();
    ImGui::BeginChild("##VisualRight", ImVec2(rightWidth, 0), false);

    const char* subTabs[] = { "Colors", "Chams", "World" };
    m_visualSubTab = StyleUI::TabBarButton("##VisualSubTabs", subTabs, 3, m_visualSubTab);

    ImGui::Spacing();
    ImGui::Spacing();

    if (m_visualSubTab == 0) {
        // Colors tab
        if (StyleUI::BeginGroupBoxFlat("ESP Colors")) {
            StyleUI::ColorEdit4("Box Color", m_espBoxColor);
            StyleUI::ColorEdit4("Name Color", m_espNameColor);
            StyleUI::EndGroupBoxFlat();
        }
    } else if (m_visualSubTab == 1) {
        // Chams tab
        if (StyleUI::BeginGroupBoxFlat("Chams")) {
            StyleUI::ToggleSwitch("Enable Chams", &m_chamsEnabled);

            if (m_chamsEnabled) {
                ImGui::Spacing();
                StyleUI::Checkbox("Visible Only", &m_chamsVisibleOnly);

                const char* materials[] = { "Flat", "Textured", "Metallic", "Glow" };
                StyleUI::Combo("Material", &m_chamsMaterialIndex, materials, 4);

                ImGui::Spacing();

                if (StyleUI::BeginGroupBoxNested("Colors")) {
                    StyleUI::ColorEdit4("Visible", m_chamsVisibleColor);
                    StyleUI::ColorEdit4("Hidden", m_chamsHiddenColor);
                    StyleUI::EndGroupBoxNested();
                }
            }

            StyleUI::EndGroupBoxFlat();
        }
    } else {
        // World tab
        if (StyleUI::BeginGroupBoxFlat("World ESP")) {
            StyleUI::Checkbox("Item ESP", &m_espEnabled);
            StyleUI::Checkbox("Vehicle ESP", &m_espBox);
            StyleUI::EndGroupBoxFlat();
        }
    }

    ImGui::EndChild();
}

void LargeDemoScreen::RenderMiscTab(float width, float height) {
    float leftWidth = LEFT_PANEL_WIDTH;
    float rightWidth = width - leftWidth - PADDING;

    // Left column
    ImGui::BeginChild("##MiscLeft", ImVec2(leftWidth, 0), false);

    if (StyleUI::BeginGroupBoxFlat("Movement")) {
        StyleUI::ToggleSwitch("Bunny Hop", &m_bunnyHop);
        StyleUI::ToggleSwitch("Auto Strafe", &m_autoStrafe);
        StyleUI::EndGroupBoxFlat();
    }

    ImGui::Spacing();

    if (StyleUI::BeginGroupBoxFlat("Visual Removal")) {
        StyleUI::ToggleSwitch("No Flash", &m_noFlash);
        StyleUI::ToggleSwitch("No Smoke", &m_noSmoke);

        if (!m_noFlash) {
            ImGui::Spacing();
            StyleUI::SliderFloat("Flash Alpha", &m_flashAlpha, 0.0f, 1.0f, "%.2f");
        }
        StyleUI::EndGroupBoxFlat();
    }

    ImGui::EndChild();

    // Right column
    ImGui::SameLine();
    ImGui::BeginChild("##MiscRight", ImVec2(rightWidth, 0), false);

    const char* subTabs[] = { "Radar", "Skins", "Other" };
    m_miscSubTab = StyleUI::TabBarButton("##MiscSubTabs", subTabs, 3, m_miscSubTab);

    ImGui::Spacing();
    ImGui::Spacing();

    if (m_miscSubTab == 0) {
        if (StyleUI::BeginGroupBoxFlat("Radar")) {
            StyleUI::ToggleSwitch("Enable Radar", &m_radarEnabled);

            if (m_radarEnabled) {
                ImGui::Spacing();
                StyleUI::SliderFloat("Size", &m_radarSize, 100.0f, 400.0f, "%.0f");
                StyleUI::SliderFloat("Zoom", &m_radarZoom, 0.5f, 3.0f, "%.1fx");
            }
            StyleUI::EndGroupBoxFlat();
        }
    }

    ImGui::EndChild();
}

void LargeDemoScreen::RenderSettingsTab(float width, float height) {
    float leftWidth = LEFT_PANEL_WIDTH;
    float rightWidth = width - leftWidth - PADDING;

    // Left column
    ImGui::BeginChild("##SettingsLeft", ImVec2(leftWidth, 0), false);

    if (StyleUI::BeginGroupBoxFlat("Config")) {
        StyleUI::ToggleSwitch("Save on Exit", &m_saveOnExit);
        StyleUI::ToggleSwitch("Load on Start", &m_loadOnStart);

        ImGui::Spacing();
        ImGui::Spacing();

        if (StyleUI::ButtonGradient("Save Config", ImVec2(ImGui::GetContentRegionAvail().x, 32))) {
            // Save action
        }

        ImGui::Spacing();

        if (StyleUI::ButtonOutline("Load Config", ImVec2(ImGui::GetContentRegionAvail().x, 32))) {
            // Load action
        }

        StyleUI::EndGroupBoxFlat();
    }

    ImGui::EndChild();

    // Right column
    ImGui::SameLine();
    ImGui::BeginChild("##SettingsRight", ImVec2(rightWidth, 0), false);

    const char* subTabs[] = { "Theme", "Language", "About" };
    m_settingsSubTab = StyleUI::TabBarButton("##SettingsSubTabs", subTabs, 3, m_settingsSubTab);

    ImGui::Spacing();
    ImGui::Spacing();

    if (m_settingsSubTab == 0) {
        if (StyleUI::BeginGroupBoxFlat("Theme")) {
            const char* themes[] = { "Dark Blue", "Dark Purple", "Dark Green", "Dark Red" };
            int prevTheme = m_themeIndex;
            StyleUI::Combo("Color Theme", &m_themeIndex, themes, 4);

            if (m_themeIndex != prevTheme) {
                switch (m_themeIndex) {
                    case 0: StyleUI::SetColorScheme(StyleUI::GetDarkBlueScheme()); break;
                    case 1: StyleUI::SetColorScheme(StyleUI::GetDarkPurpleScheme()); break;
                    case 2: StyleUI::SetColorScheme(StyleUI::GetDarkGreenScheme()); break;
                    case 3: StyleUI::SetColorScheme(StyleUI::GetDarkRedScheme()); break;
                }
            }
            StyleUI::EndGroupBoxFlat();
        }
    } else if (m_settingsSubTab == 1) {
        if (StyleUI::BeginGroupBoxFlat("Language")) {
            if (StyleUI::LanguageCombo("Interface Language", m_currentLanguage)) {
                i18n::SetLanguage(m_currentLanguage);
            }
            StyleUI::EndGroupBoxFlat();
        }
    } else {
        if (StyleUI::BeginGroupBoxFlat("About")) {
            ImGui::Text("Demo Menu v1.0.0");
            ImGui::Spacing();
            ImGui::TextWrapped("This is a demonstration of the NEXO-style large menu layout.");
            ImGui::Spacing();
            ImGui::Text("Built with ImGui + StyleUI");
            StyleUI::EndGroupBoxFlat();
        }
    }

    ImGui::EndChild();
}
