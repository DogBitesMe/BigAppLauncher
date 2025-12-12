#include "LoginScreen.h"
#include "../Theme.h"
#include "../Widgets.h"
#include "../StyleUI.h"
#include "../../i18n/Localization.h"
#include <imgui.h>

void LoginScreen::SetLoginMode(int mode) {
    m_loginMode = static_cast<LoginMode>(mode);
    // Reset to appropriate tab when mode changes
    if (m_loginMode == LoginMode::CardKey) {
        m_currentTab = Tab::CardKey;
    } else {
        m_currentTab = Tab::Account;
    }
    m_errorMessage.clear();
    m_successMessage.clear();
}

void LoginScreen::SetLoginResult(int result) {
    m_loginResult = static_cast<LoginResult>(result);
}

void LoginScreen::Update(float deltaTime) {
    // Handle login success delay
    if (m_loginPending) {
        m_loginDelayTimer -= deltaTime;
        if (m_loginDelayTimer <= 0.0f) {
            m_loginPending = false;
            m_isLoggedIn = true;
        }
    }
}

void LoginScreen::HandleLoginAttempt() {
    m_errorMessage.clear();
    m_successMessage.clear();

    switch (m_loginResult) {
        case LoginResult::Success:
            m_successMessage = i18n::T("login.success");
            m_loginPending = true;
            m_loginDelayTimer = 1.0f;  // 1 second delay
            break;

        case LoginResult::ConnectionError:
            m_errorMessage = i18n::T("login.error_connection");
            break;

        case LoginResult::WrongCredentials:
            m_errorMessage = i18n::T("login.error_credentials");
            break;

        case LoginResult::Expired:
            m_errorMessage = i18n::T("login.error_expired");
            break;
    }
}

void LoginScreen::Render(int windowWidth, int windowHeight) {
    // Panel dimensions (larger default size)
    const float panelWidth = 480.0f;
    const float panelHeight = 560.0f;

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
        ImGui::SetCursorPos(ImVec2(panelWidth - 105, 10));
        ImGui::PushItemWidth(95);
        if (Widgets::LanguageSelector("##lang", m_currentLanguage)) {
            i18n::SetLanguage(m_currentLanguage);
        }
        ImGui::PopItemWidth();

        // App title
        ImGui::SetCursorPos(ImVec2(0, 30));
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Assuming first font is larger
        Widgets::TextCentered(i18n::T("app_name"));
        ImGui::PopFont();

        ImGui::Spacing();
        ImGui::Spacing();

        // Build tab array based on login mode
        const char* tabs[4];
        int tabCount = 0;

        if (m_loginMode == LoginMode::CardKey) {
            tabs[tabCount++] = i18n::T("login.tab_cardkey");
        } else {
            tabs[tabCount++] = i18n::T("login.tab_account");
        }
        tabs[tabCount++] = i18n::T("login.tab_register");
        tabs[tabCount++] = i18n::T("login.tab_password");

        ImGui::SetCursorPosX(20);
        ImGui::PushItemWidth(panelWidth - 40);

        // Map current tab to tab index
        int tabIndex = 0;
        if (m_loginMode == LoginMode::CardKey) {
            switch (m_currentTab) {
                case Tab::CardKey: tabIndex = 0; break;
                case Tab::Register: tabIndex = 1; break;
                case Tab::Password: tabIndex = 2; break;
                default: tabIndex = 0; break;
            }
        } else {
            switch (m_currentTab) {
                case Tab::Account: tabIndex = 0; break;
                case Tab::Register: tabIndex = 1; break;
                case Tab::Password: tabIndex = 2; break;
                default: tabIndex = 0; break;
            }
        }

        int newTabIndex = StyleUI::TabBarSmall("LoginTabs", tabs, tabCount, tabIndex);

        // Map back to Tab enum
        if (m_loginMode == LoginMode::CardKey) {
            switch (newTabIndex) {
                case 0: m_currentTab = Tab::CardKey; break;
                case 1: m_currentTab = Tab::Register; break;
                case 2: m_currentTab = Tab::Password; break;
            }
        } else {
            switch (newTabIndex) {
                case 0: m_currentTab = Tab::Account; break;
                case 1: m_currentTab = Tab::Register; break;
                case 2: m_currentTab = Tab::Password; break;
            }
        }

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

    StyleUI::TextInput(i18n::T("login.input_cardkey"), m_cardKey, sizeof(m_cardKey),
                       StyleUI::TextInputLabelMode::Above, i18n::T("login.input_cardkey"));

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // Login button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    bool disabled = m_loginPending;

    if (disabled) {
        ImGui::BeginDisabled();
    }

    if (StyleUI::ButtonGradient(i18n::T("login.btn_login"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        // Validate
        if (strlen(m_cardKey) == 0) {
            m_errorMessage = i18n::T("login.error_empty_cardkey");
        } else {
            HandleLoginAttempt();
        }
    }

    if (disabled) {
        ImGui::EndDisabled();
    }
}

void LoginScreen::RenderAccountTab() {
    ImGui::Spacing();

    StyleUI::TextInput(i18n::T("login.input_username"), m_username, sizeof(m_username),
                       StyleUI::TextInputLabelMode::Above, i18n::T("login.input_username"));

    ImGui::Spacing();

    StyleUI::PasswordInput(i18n::T("login.input_password"), m_password, sizeof(m_password),
                           StyleUI::TextInputLabelMode::Above, i18n::T("login.input_password"));

    ImGui::Spacing();

    // Remember me checkbox (classic style - checkbox on left)
    StyleUI::CheckboxClassic(i18n::T("login.remember_me"), &m_rememberMe);

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 80);
    ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors::Primary);
    ImGui::Text("%s", i18n::T("login.forgot_password"));
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Spacing();

    // Login button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    bool disabled = m_loginPending;

    if (disabled) {
        ImGui::BeginDisabled();
    }

    if (StyleUI::ButtonGradient(i18n::T("login.btn_login"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        if (strlen(m_username) == 0) {
            m_errorMessage = i18n::T("login.error_empty_username");
        } else if (strlen(m_password) == 0) {
            m_errorMessage = i18n::T("login.error_empty_password");
        } else {
            HandleLoginAttempt();
        }
    }

    if (disabled) {
        ImGui::EndDisabled();
    }
}

void LoginScreen::RenderRegisterTab() {
    ImGui::Spacing();

    StyleUI::TextInput(i18n::T("login.input_username"), m_username, sizeof(m_username),
                       StyleUI::TextInputLabelMode::Above, i18n::T("login.input_username"));

    ImGui::Spacing();

    StyleUI::TextInput(i18n::T("login.input_email"), m_email, sizeof(m_email),
                       StyleUI::TextInputLabelMode::Above, i18n::T("login.input_email"));

    ImGui::Spacing();

    StyleUI::PasswordInput(i18n::T("login.input_password"), m_password, sizeof(m_password),
                           StyleUI::TextInputLabelMode::Above, i18n::T("login.input_password"));

    ImGui::Spacing();

    StyleUI::PasswordInput(i18n::T("login.input_confirm_password"), m_confirmPassword, sizeof(m_confirmPassword),
                           StyleUI::TextInputLabelMode::Above, i18n::T("login.input_confirm_password"));

    ImGui::Spacing();
    ImGui::Spacing();

    // Register button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    if (StyleUI::ButtonGradient(i18n::T("login.btn_register"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        if (strlen(m_username) == 0) {
            m_errorMessage = i18n::T("login.error_empty_username");
        } else if (strlen(m_password) == 0) {
            m_errorMessage = i18n::T("login.error_empty_password");
        } else if (strcmp(m_password, m_confirmPassword) != 0) {
            m_errorMessage = i18n::T("login.error_password_mismatch");
        } else {
            m_errorMessage.clear();
            m_successMessage = i18n::T("login.register_success");
        }
    }
}

void LoginScreen::RenderPasswordTab() {
    ImGui::Spacing();
    ImGui::Spacing();

    StyleUI::PasswordInput(i18n::T("login.input_old_password"), m_oldPassword, sizeof(m_oldPassword),
                           StyleUI::TextInputLabelMode::Above, i18n::T("login.input_old_password"));

    ImGui::Spacing();

    StyleUI::PasswordInput(i18n::T("login.input_new_password"), m_newPassword, sizeof(m_newPassword),
                           StyleUI::TextInputLabelMode::Above, i18n::T("login.input_new_password"));

    ImGui::Spacing();

    StyleUI::PasswordInput(i18n::T("login.input_confirm_password"), m_confirmPassword, sizeof(m_confirmPassword),
                           StyleUI::TextInputLabelMode::Above, i18n::T("login.input_confirm_password"));

    ImGui::Spacing();
    ImGui::Spacing();

    // Change password button
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    if (StyleUI::ButtonGradient(i18n::T("login.btn_change_password"), ImVec2(buttonWidth, Theme::Size::ButtonHeight))) {
        if (strlen(m_oldPassword) == 0 || strlen(m_newPassword) == 0) {
            m_errorMessage = i18n::T("login.error_empty_password");
        } else if (strcmp(m_newPassword, m_confirmPassword) != 0) {
            m_errorMessage = i18n::T("login.error_password_mismatch");
        } else {
            m_errorMessage.clear();
            m_successMessage = i18n::T("login.password_changed");
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
    m_loginPending = false;
    m_loginDelayTimer = 0.0f;
    m_errorMessage.clear();
    m_successMessage.clear();
}
