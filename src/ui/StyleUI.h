#pragma once

#include <imgui.h>
#include <string>
#include <unordered_map>

namespace StyleUI {

//-----------------------------------------------------------------------------
// Theme Structures
//-----------------------------------------------------------------------------

struct ColorScheme {
    // Primary colors
    ImVec4 Primary;
    ImVec4 PrimaryHover;
    ImVec4 PrimaryActive;

    // Accent colors
    ImVec4 Accent;
    ImVec4 AccentHover;

    // Background
    ImVec4 Background;
    ImVec4 BackgroundAlt;
    ImVec4 GroupBoxHeader;
    ImVec4 GroupBoxBody;

    // Text
    ImVec4 Text;
    ImVec4 TextSecondary;
    ImVec4 TextDisabled;

    // Controls
    ImVec4 ToggleOn;
    ImVec4 ToggleOff;
    ImVec4 SliderTrack;
    ImVec4 SliderFill;
    ImVec4 SliderGrab;

    // Borders
    ImVec4 Border;
    ImVec4 BorderHover;

    // Status
    ImVec4 Success;
    ImVec4 Warning;
    ImVec4 Error;
};

struct SizeConfig {
    float Rounding = 6.0f;
    float BorderWidth = 1.0f;
    float GroupBoxHeaderHeight = 32.0f;
    float ToggleWidth = 44.0f;
    float ToggleHeight = 22.0f;
    float SliderHeight = 20.0f;
    float SliderGrabWidth = 14.0f;
    float TabLargeHeight = 60.0f;
    float TabSmallHeight = 32.0f;
    float ButtonHeight = 32.0f;
    float ItemSpacing = 8.0f;
    float FramePadding = 8.0f;
    float CheckboxSize = 18.0f;
};

//-----------------------------------------------------------------------------
// Theme Management
//-----------------------------------------------------------------------------

// Built-in color schemes
ColorScheme GetDarkBlueScheme();
ColorScheme GetDarkPurpleScheme();
ColorScheme GetDarkGreenScheme();
ColorScheme GetDarkRedScheme();

// Theme control
void SetColorScheme(const ColorScheme& scheme);
void SetSizeConfig(const SizeConfig& config);
ColorScheme& GetColorScheme();
SizeConfig& GetSizeConfig();

// Animation helper
float Animate(ImGuiID id, float target, float speed = 10.0f);
void UpdateAnimations(float deltaTime);

//-----------------------------------------------------------------------------
// Hotkey Binding
//-----------------------------------------------------------------------------

struct HotkeyBinding {
    int key = 0;           // Virtual key code (0 = unbound)
    bool ctrl = false;
    bool alt = false;
    bool shift = false;

    bool IsValid() const { return key != 0; }
    void Clear() { key = 0; ctrl = false; alt = false; shift = false; }
};

// Get display string for a hotkey (e.g., "Ctrl+Shift+F1")
const char* GetHotkeyDisplayString(const HotkeyBinding& binding);

//-----------------------------------------------------------------------------
// Widgets
//-----------------------------------------------------------------------------

// GroupBox - Styled container with header
bool BeginGroupBox(const char* label, const ImVec2& size = ImVec2(0, 0));
void EndGroupBox();
bool BeginGroupBoxEx(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0));

// Toggle Switch - Mobile-style on/off switch (text left, switch right)
bool ToggleSwitch(const char* label, bool* v);
bool ToggleSwitchEx(const char* label, bool* v, const ImVec4& onColor, const ImVec4& offColor);

// Checkbox - Styled checkbox
bool Checkbox(const char* label, bool* v);                    // Text left, checkbox right
bool CheckboxClassic(const char* label, bool* v);             // Checkbox left, text right

// Sliders - Gradient-filled sliders
bool SliderFloat(const char* label, float* v, float min, float max, const char* format = "%.1f");
bool SliderInt(const char* label, int* v, int min, int max, const char* format = "%d");
bool SliderFloatGradient(const char* label, float* v, float min, float max,
                         const ImVec4& leftColor, const ImVec4& rightColor, const char* format = "%.1f");

// Tab Systems

// Tab indicator style configuration
struct TabStyle {
    bool gradientIndicator = false;              // Use gradient for indicator
    ImVec4 indicatorGradientStart = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);  // Left color
    ImVec4 indicatorGradientEnd = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);       // Right color
    float indicatorWidthRatio = 1.0f;            // 0.0-1.0, ratio of tab width (1.0 = full width)
    bool indicatorCentered = true;               // Center the indicator
    float indicatorHeight = 3.0f;                // Indicator thickness
    float indicatorPadding = 4.0f;               // Padding from tab edges when full width
};

// Large tabs with icons and text (for main sections)
int TabBarLarge(const char* id, const char** icons, const char** labels, int count, int current);
int TabBarLargeEx(const char* id, const char** icons, const char** labels, int count, int current, const TabStyle& style);

// Small tabs (text only or with icons)
int TabBarSmall(const char* id, const char** labels, int count, int current);
int TabBarSmallIcon(const char* id, const char** icons, const char** labels, int count, int current);

// RadioButton - Two styles
// Segmented control (horizontal tab-like buttons)
int RadioButtonGroup(const char* id, const char** labels, int count, int current);
int RadioButtonGroupIcon(const char* id, const char** icons, const char** labels, int count, int current);

// Classic style (circle indicator)
bool RadioButton(const char* label, int* v, int buttonValue);       // Text left, indicator right
bool RadioButtonClassic(const char* label, int* v, int buttonValue); // Indicator left, text right

// SubTab - Smaller tabs for sub-page navigation
enum class SubTabStyle {
    Underline = 0,  // Underline indicator below text
    Pill = 1        // Pill/rounded background fill
};

// Text-based sub-tabs with optional icons
int SubTab(const char* id, const char** labels, int count, int current, SubTabStyle style = SubTabStyle::Underline);
int SubTabIcon(const char* id, const char** icons, const char** labels, int count, int current, SubTabStyle style = SubTabStyle::Underline);

// Hotkey Input
bool HotkeyInput(const char* label, HotkeyBinding* binding);

// Buttons
bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));
bool ButtonGradient(const char* label, const ImVec2& size = ImVec2(0, 0));
bool ButtonOutline(const char* label, const ImVec2& size = ImVec2(0, 0));
bool ButtonIcon(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0));

// ComboBox / Dropdown
bool Combo(const char* label, int* current, const char** items, int count);
bool ComboEx(const char* label, int* current, const char** items, int count, float width);

// Color Picker
bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);

// Separator with text
void SeparatorText(const char* text);

// Label with value aligned right
void LabelValue(const char* label, const char* value);

} // namespace StyleUI
