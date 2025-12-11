#include "LoginScreen.h"
#include "../Theme.h"
#include "../Widgets.h"
#include "../../i18n/Localization.h"
#include <imgui.h>

void LoginScreen::Render(int windowWidth, int windowHeight) {
    // Panel dimensions
    const float panelWidth = 420.0f;
    const float panelHeight = 480.0f;

    // Center the panel
    float panelX = (windowWidth - panelWidth) * 0.5f;
    float panelY = (windowHeight - panelHeight) * 0.5f;

    // Panel background is now handled by Application with blur effect
    ImVec2 panelPos(panelX, panelY);

    // Set window position and size
    ImGui::SetNextWindowPos(panelPos);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight));

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground;

    if (ImGui::Begin("LoginPanel", nullptr, windowFlags)) {
        // Language selector at top right
        ImGui::SetCursorPos(ImVec2(panelWidth - 120, 10));
        if (Widgets::LanguageSelector("##lang", m_currentLanguage)) {
            i18n::SetLanguage(m_currentLanguage);
        }

        // App title
        ImGui::SetCursorPos(ImVec2(0, 30));
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Assuming first font is larger
        Widgets::TextCentered(i18n::T("app_name"));
        ImGui::PopFont();

        ImGui::Spacing();
        ImGui::Spacing();

        // Tab bar
        const char* tabs[] = {
            i18n::T("login.tab_cardkey"),
            i18n::T("login.tab_account"),
            i18n::T("login.tab_register"),
            i18n::T("login.tab_password")
        };

        ImGui::SetCursorPosX(20);
        ImGui::PushItemWidth(panelWidth - 40);

        int tabIndex = static_cast<int>(m_currentTab);
        tabIndex = Widgets::TabBar("LoginTabs", tabs, 4, tabIndex);
        m_currentTab = static_cast<Tab>(tabIndex);

        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Spacing();

        // Content area
        ImGui::SetCursorPosX(30);
        ImGui::BeginGroup();
        ImGui::PushItemWidth(panelWidth - 60);

        // Clear messages when switching tabs
        static Tab lastTab = m_currentTab;
        if (lastTab != m_currentTab) {
            m_errorMessage.clear();
            m_successMessage.clear();
            lastTab = m_currentTab;
        }

        // Render current tab content
        switch (m_currentTab) {
            case Tab::CardKey:
                RenderCardKeyTab();
                break;
            case Tab::Account:
                RenderAccountTab();
                break;
            case Tab::Register:
                RenderRegisterTab();
                break;
            case Tab::Password:
                RenderPasswordTab();
                break;
        }

        ImGui::PopItemWidth();
        ImGui::EndGroup();

        // Error/Success messages
        if (!m_errorMessage.empty()) {
            ImGui::SetCursorPos(ImVec2(30, panelHeight - 60));
            Widgets::MessageBox(m_errorMessage.c_str(), Theme::Colors::Error);
        }
        if (!m_successMessage.empty()) {
            ImGui::SetCursorPos(ImVec2(30, panelHeight - 60));
            Widgets::MessageBox(m_successMessage.c_str(), Theme::Colors::Success);
        }
    }
    ImGui::End();
}

void LoginScreen::RenderCardKeyTab() {
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_cardkey"));
    Widgets::InputText("##cardkey", m_cardKey, sizeof(m_cardKey), i18n::T("login.input_cardkey"));

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // Login button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    if (Widgets::GradientButton(i18n::T("login.btn_login"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        // Validate
        if (strlen(m_cardKey) == 0) {
            m_errorMessage = i18n::T("login.error_empty_cardkey");
        } else {
            // Mock login success
            m_errorMessage.clear();
            m_successMessage = "Login successful! (Mock)";
            // m_isLoggedIn = true; // Enable this to proceed to next screen
        }
    }
}

void LoginScreen::RenderAccountTab() {
    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_username"));
    Widgets::InputText("##username", m_username, sizeof(m_username), i18n::T("login.input_username"));

    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_password"));
    Widgets::InputPassword("##password", m_password, sizeof(m_password), i18n::T("login.input_password"));

    ImGui::Spacing();

    // Remember me checkbox
    ImGui::Checkbox(i18n::T("login.remember_me"), &m_rememberMe);

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 80);
    ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors::Primary);
    ImGui::Text("%s", i18n::T("login.forgot_password"));
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Spacing();

    // Login button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    if (Widgets::GradientButton(i18n::T("login.btn_login"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        if (strlen(m_username) == 0) {
            m_errorMessage = i18n::T("login.error_empty_username");
        } else if (strlen(m_password) == 0) {
            m_errorMessage = i18n::T("login.error_empty_password");
        } else {
            m_errorMessage.clear();
            m_successMessage = "Login successful! (Mock)";
        }
    }
}

void LoginScreen::RenderRegisterTab() {
    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_username"));
    Widgets::InputText("##reg_username", m_username, sizeof(m_username), i18n::T("login.input_username"));

    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_email"));
    Widgets::InputText("##reg_email", m_email, sizeof(m_email), i18n::T("login.input_email"));

    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_password"));
    Widgets::InputPassword("##reg_password", m_password, sizeof(m_password), i18n::T("login.input_password"));

    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_confirm_password"));
    Widgets::InputPassword("##reg_confirm", m_confirmPassword, sizeof(m_confirmPassword), i18n::T("login.input_confirm_password"));

    ImGui::Spacing();
    ImGui::Spacing();

    // Register button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    if (Widgets::GradientButton(i18n::T("login.btn_register"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        if (strlen(m_username) == 0) {
            m_errorMessage = i18n::T("login.error_empty_username");
        } else if (strlen(m_password) == 0) {
            m_errorMessage = i18n::T("login.error_empty_password");
        } else if (strcmp(m_password, m_confirmPassword) != 0) {
            m_errorMessage = i18n::T("login.error_password_mismatch");
        } else {
            m_errorMessage.clear();
            m_successMessage = "Registration successful! (Mock)";
        }
    }
}

void LoginScreen::RenderPasswordTab() {
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_old_password"));
    Widgets::InputPassword("##old_password", m_oldPassword, sizeof(m_oldPassword), i18n::T("login.input_old_password"));

    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_new_password"));
    Widgets::InputPassword("##new_password", m_newPassword, sizeof(m_newPassword), i18n::T("login.input_new_password"));

    ImGui::Spacing();

    ImGui::Text("%s", i18n::T("login.input_confirm_password"));
    Widgets::InputPassword("##confirm_new", m_confirmPassword, sizeof(m_confirmPassword), i18n::T("login.input_confirm_password"));

    ImGui::Spacing();
    ImGui::Spacing();

    // Change password button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    if (Widgets::GradientButton(i18n::T("login.btn_change_password"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        if (strlen(m_oldPassword) == 0 || strlen(m_newPassword) == 0) {
            m_errorMessage = i18n::T("login.error_empty_password");
        } else if (strcmp(m_newPassword, m_confirmPassword) != 0) {
            m_errorMessage = i18n::T("login.error_password_mismatch");
        } else {
            m_errorMessage.clear();
            m_successMessage = "Password changed! (Mock)";
        }
    }
}

void LoginScreen::Reset() {
    m_currentTab = Tab::CardKey;
    m_cardKey[0] = '\0';
    m_username[0] = '\0';
    m_password[0] = '\0';
    m_confirmPassword[0] = '\0';
    m_email[0] = '\0';
    m_oldPassword[0] = '\0';
    m_newPassword[0] = '\0';
    m_rememberMe = false;
    m_isLoggedIn = false;
    m_errorMessage.clear();
    m_successMessage.clear();
}
