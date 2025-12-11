#pragma once

#include <string>

class LoginScreen {
public:
    LoginScreen() = default;
    ~LoginScreen() = default;

    // Called every frame
    void Render(int windowWidth, int windowHeight);

    // Check if user completed login
    bool IsLoggedIn() const { return m_isLoggedIn; }

    // Reset state
    void Reset();

private:
    void RenderCardKeyTab();
    void RenderAccountTab();
    void RenderRegisterTab();
    void RenderPasswordTab();

    // Tab state
    enum class Tab {
        CardKey = 0,
        Account,
        Register,
        Password
    };
    Tab m_currentTab = Tab::CardKey;

    // Input buffers
    char m_cardKey[128] = "";
    char m_username[64] = "";
    char m_password[64] = "";
    char m_confirmPassword[64] = "";
    char m_email[128] = "";
    char m_oldPassword[64] = "";
    char m_newPassword[64] = "";

    // State
    bool m_rememberMe = false;
    bool m_isLoggedIn = false;
    std::string m_errorMessage;
    std::string m_successMessage;

    // Language
    std::string m_currentLanguage = "zh-CN";
};
