#pragma once

#include <imgui.h>
#include <string>
#include <unordered_map>

// Forward declarations for DirectX types
struct ID3D11ShaderResourceView;

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
float Animate(ImGuiID id, float target, float speed = 10.0f);         // Spring physics (can oscillate)
float AnimateLinear(ImGuiID id, float target, float speed = 10.0f);   // Linear interpolation (smooth, no oscillation)
void UpdateAnimations(float deltaTime);

//-----------------------------------------------------------------------------
// Glass Effect System
//-----------------------------------------------------------------------------

// Glass effect configuration
struct GlassConfig {
    float alpha = 0.7f;                           // Background transparency (0.0-1.0)
    float rounding = 8.0f;                        // Corner rounding
    ImU32 tintColor = IM_COL32(15, 15, 25, 160);  // Overlay tint color
    ImU32 borderColor = IM_COL32(255, 255, 255, 30);  // Border glow color
    float borderThickness = 1.5f;                 // Border thickness
    bool drawBorder = true;                       // Whether to draw border
};

// Set the blurred background texture for glass effects
// Call this every frame from Application before rendering UI
void SetBlurredBackgroundSRV(ID3D11ShaderResourceView* srv, int screenWidth, int screenHeight);

// Get the current blurred background texture (nullptr if not set)
ID3D11ShaderResourceView* GetBlurredBackgroundSRV();

// Check if glass effect is available (blurred texture is set)
bool IsGlassAvailable();

// Draw a glass-effect rectangle at the specified position
// Uses the blurred background texture to create frosted glass appearance
void DrawGlassRect(const ImVec2& pos, const ImVec2& size, const GlassConfig& config = GlassConfig());

// Simplified version with common parameters
void DrawGlassRectSimple(const ImVec2& pos, const ImVec2& size,
                         float alpha = 0.7f, float rounding = 8.0f);

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

// Glass GroupBox variants - Uses frosted glass effect for background
// Requires SetBlurredBackgroundSRV to be called each frame
bool BeginGroupBoxGlass(const char* label, const ImVec2& size = ImVec2(0, 0), const GlassConfig& config = GlassConfig());
bool BeginGroupBoxExGlass(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0), const GlassConfig& config = GlassConfig());
void EndGroupBoxGlass();

// Flat GroupBox variants - Minimalist style with no header bar, just text title
// Light gray background, no border, large rounded corners
bool BeginGroupBoxFlat(const char* label, const ImVec2& size = ImVec2(0, 0));
bool BeginGroupBoxFlatEx(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0));
void EndGroupBoxFlat();

// Nested GroupBox - Darker background for sub-sections within Flat GroupBox
// Use inside BeginGroupBoxFlat for hierarchical grouping
bool BeginGroupBoxNested(const char* label, const ImVec2& size = ImVec2(0, 0));
bool BeginGroupBoxNestedEx(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0));
void EndGroupBoxNested();

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
    bool useGradient = true;                     // Use gradient background (false = solid color)
    bool scrollable = false;                     // Enable horizontal scrolling for overflow
    float minTabWidth = 80.0f;                   // Minimum tab width before scrolling
    float scrollArrowWidth = 24.0f;              // Width of scroll arrow buttons
};

// Small tab style configuration
struct SmallTabStyle {
    bool allowWrap = false;                      // Allow wrapping to multiple lines
    float tabPadding = 16.0f;                    // Horizontal padding per tab
    float tabSpacing = 8.0f;                     // Space between tabs
    float rowSpacing = 4.0f;                     // Space between rows (when wrapping)
};

// Large tabs with icons and text (for main sections)
int TabBarLarge(const char* id, const char** icons, const char** labels, int count, int current);
int TabBarLargeEx(const char* id, const char** icons, const char** labels, int count, int current, const TabStyle& style);

// Small tabs (text only or with icons)
int TabBarSmall(const char* id, const char** labels, int count, int current);
int TabBarSmallIcon(const char* id, const char** icons, const char** labels, int count, int current);
int TabBarSmallEx(const char* id, const char** labels, int count, int current, const SmallTabStyle& style);
int TabBarSmallIconEx(const char* id, const char** icons, const char** labels, int count, int current, const SmallTabStyle& style);

// Pill tabs - Capsule-shaped container with underline indicator for selected item
// Used for main navigation in NEXO-style menus
int TabBarPill(const char* id, const char** labels, int count, int current);
int TabBarPillIcon(const char* id, const char** icons, const char** labels, int count, int current);

// Button tabs - Simple button-style tabs with semi-transparent background on selection
// Used for secondary navigation within sections
int TabBarButton(const char* id, const char** labels, int count, int current);
int TabBarButtonIcon(const char* id, const char** icons, const char** labels, int count, int current);

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
    Pill = 1,       // Pill/rounded background fill (solid)
    Gradient = 2    // Gradient background like Tab
};

// Text-based sub-tabs with optional icons
int SubTab(const char* id, const char** labels, int count, int current, SubTabStyle style = SubTabStyle::Underline);
int SubTabIcon(const char* id, const char** icons, const char** labels, int count, int current, SubTabStyle style = SubTabStyle::Underline);

// Hotkey Input
bool HotkeyInput(const char* label, HotkeyBinding* binding);

// TextInput - Text input field with configurable label placement
enum class TextInputLabelMode {
    Above,      // Label above input field
    Below,      // Label below input field
    Hidden,     // No visible label (use for tooltip only)
    Placeholder // Label as placeholder text inside input
};

bool TextInput(const char* label, char* buf, size_t bufSize,
               TextInputLabelMode labelMode = TextInputLabelMode::Above,
               const char* placeholder = nullptr);
bool PasswordInput(const char* label, char* buf, size_t bufSize,
                   TextInputLabelMode labelMode = TextInputLabelMode::Above,
                   const char* placeholder = nullptr);

// Buttons
bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));
bool ButtonGradient(const char* label, const ImVec2& size = ImVec2(0, 0));
bool ButtonOutline(const char* label, const ImVec2& size = ImVec2(0, 0));
bool ButtonIcon(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0));

// ComboBox / Dropdown
bool Combo(const char* label, int* current, const char** items, int count);
bool ComboEx(const char* label, int* current, const char** items, int count, float width);

// Language selector dropdown (for i18n)
// Returns true if language was changed
bool LanguageCombo(const char* label, std::string& currentLang, float width = 0.0f);

// Color Picker
bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);

// Separator with text
void SeparatorText(const char* text);

// Label with value aligned right
void LabelValue(const char* label, const char* value);

} // namespace StyleUI
