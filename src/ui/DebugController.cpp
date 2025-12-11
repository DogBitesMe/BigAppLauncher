#include "DebugController.h"
#include "screens/LoginScreen.h"
#include "Theme.h"
#include <imgui.h>

void DebugController::Render(LoginScreen* loginScreen) {
    if (!showDebugWindow) return;

    ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 480), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Debug Controller", &showDebugWindow, flags)) {
        ImGui::Text("Login Settings");
        ImGui::Separator();

        // Login Mode Selection
        ImGui::Text("Login Mode:");
        int modeIndex = static_cast<int>(loginMode);
        if (ImGui::RadioButton("CardKey", &modeIndex, 0)) {
            loginMode = LoginMode::CardKey;
            if (loginScreen) loginScreen->SetLoginMode(static_cast<int>(loginMode));
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Account", &modeIndex, 1)) {
            loginMode = LoginMode::Account;
            if (loginScreen) loginScreen->SetLoginMode(static_cast<int>(loginMode));
        }

        ImGui::Spacing();

        // Login Result Selection
        ImGui::Text("Login Result:");
        int resultIndex = static_cast<int>(loginResult);

        if (ImGui::RadioButton("Success", &resultIndex, 0)) {
            loginResult = LoginResult::Success;
            if (loginScreen) loginScreen->SetLoginResult(static_cast<int>(loginResult));
        }
        if (ImGui::RadioButton("Connection Error", &resultIndex, 1)) {
            loginResult = LoginResult::ConnectionError;
            if (loginScreen) loginScreen->SetLoginResult(static_cast<int>(loginResult));
        }
        if (ImGui::RadioButton("Wrong Credentials", &resultIndex, 2)) {
            loginResult = LoginResult::WrongCredentials;
            if (loginScreen) loginScreen->SetLoginResult(static_cast<int>(loginResult));
        }
        if (ImGui::RadioButton("Expired", &resultIndex, 3)) {
            loginResult = LoginResult::Expired;
            if (loginScreen) loginScreen->SetLoginResult(static_cast<int>(loginResult));
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Skip entry page option
        ImGui::Checkbox("Skip Entry Page", &skipEntryPage);

        ImGui::Spacing();

        // Toggle debug window hint
        ImGui::TextDisabled("Press F12 to toggle this window");
    }
    ImGui::End();
}
