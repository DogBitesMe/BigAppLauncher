#pragma once

#include <string>

class LoginScreen {
public:
    // Login mode (controlled by DebugController)
    enum class LoginMode {
        CardKey = 0,
        Account = 1
    };

    // Login result (controlled by DebugController)
    enum class LoginResult {
        Success = 0,
        ConnectionError = 1,
        WrongCredentials = 2,
        Expired = 3
    };

    LoginScreen() = default;
    ~LoginScreen() = default;

    // Called every frame
    void Render(int windowWidth, int windowHeight);

    // Check if user completed login
    bool IsLoggedIn() const { return m_isLoggedIn; }

    // Reset state
    void Reset();

    // Debug controller interface
    void SetLoginMode(int mode);
    void SetLoginResult(int result);

    // Update (called each frame for timers)
    void Update(float deltaTime);

private:
    void RenderCardKeyTab();
    void RenderAccountTab();
    void RenderRegisterTab();
    void RenderPasswordTab();
    void HandleLoginAttempt();

    // Tab state
    enum class Tab {
        CardKey = 0,
        Account,
        Register,
        Password
    };
    Tab m_currentTab = Tab::CardKey;

    // Login mode and result (from debug controller)
    LoginMode m_loginMode = LoginMode::CardKey;
    LoginResult m_loginResult = LoginResult::Success;

    // Input buffers
    char m_cardKey[128] = "XXXX-XXXX-XXXX-XXXX";
    char m_username[64] = "demo";
    char m_password[64] = "password";
    char m_confirmPassword[64] = "";
    char m_email[128] = "";
    char m_oldPassword[64] = "";
    char m_newPassword[64] = "";

    // State
    bool m_rememberMe = false;
    bool m_isLoggedIn = false;
    bool m_loginPending = false;  // Waiting for success delay
    float m_loginDelayTimer = 0.0f;
    std::string m_errorMessage;
    std::string m_successMessage;

    // Language
    std::string m_currentLanguage = "zh-CN";
};
