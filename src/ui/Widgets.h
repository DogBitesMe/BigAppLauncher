#pragma once

#include <imgui.h>
#include <string>

namespace Widgets {

// Styled button with gradient effect
bool GradientButton(const char* label, const ImVec2& size = ImVec2(0, 0));

// Text input field with placeholder
bool InputText(const char* label, char* buf, size_t bufSize, const char* placeholder = nullptr,
               ImGuiInputTextFlags flags = 0);

// Password input field (masked)
bool InputPassword(const char* label, char* buf, size_t bufSize, const char* placeholder = nullptr);

// Tab bar that returns selected index
int TabBar(const char* id, const char** labels, int count, int currentIndex);

// Centered text
void TextCentered(const char* text);

// Loading spinner
void Spinner(const char* label, float radius = 12.0f, float thickness = 3.0f);

// Draws a semi-transparent panel background (for glass effect without blur)
void DrawPanelBackground(const ImVec2& pos, const ImVec2& size, float alpha = 0.85f);

// Error/Warning/Success message box
void MessageBox(const char* message, const ImVec4& color);

// Language selector dropdown
bool LanguageSelector(const char* label, std::string& currentLang);

} // namespace Widgets
