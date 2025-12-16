#pragma once

#include <imgui.h>

// Forward declaration
class LoginScreen;

class DebugController {
public:
    // Login mode control
    enum class LoginMode {
        CardKey,
        Account
    };

    // Login result control
    enum class LoginResult {
        Success,
        ConnectionError,
        WrongCredentials,
        Expired
    };

    // Current settings
    LoginMode loginMode = LoginMode::CardKey;
    LoginResult loginResult = LoginResult::Success;

    // Debug window visibility
    bool showDebugWindow = true;

    // Skip entry page option
    bool skipEntryPage = true;

    // Render the debug controller window
    void Render(LoginScreen* loginScreen);
};
