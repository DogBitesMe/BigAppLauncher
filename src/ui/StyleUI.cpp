#include "StyleUI.h"
#include <unordered_map>
#include <algorithm>

namespace StyleUI {

//-----------------------------------------------------------------------------
// Global State
//-----------------------------------------------------------------------------

static ColorScheme g_colorScheme;
static SizeConfig g_sizeConfig;
static bool g_initialized = false;

// Animation states
struct AnimState {
    float current = 0.0f;
    float velocity = 0.0f;
};
static std::unordered_map<ImGuiID, AnimState> g_animations;
static float g_deltaTime = 1.0f / 60.0f;

//-----------------------------------------------------------------------------
// Color Schemes
//-----------------------------------------------------------------------------

ColorScheme GetDarkBlueScheme() {
    ColorScheme scheme;

    // Primary - Blue
    scheme.Primary = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    scheme.PrimaryHover = ImVec4(0.36f, 0.69f, 1.0f, 1.0f);
    scheme.PrimaryActive = ImVec4(0.16f, 0.49f, 0.88f, 1.0f);

    // Accent
    scheme.Accent = ImVec4(0.40f, 0.70f, 1.0f, 1.0f);
    scheme.AccentHover = ImVec4(0.50f, 0.80f, 1.0f, 1.0f);

    // Background
    scheme.Background = ImVec4(0.08f, 0.08f, 0.12f, 1.0f);
    scheme.BackgroundAlt = ImVec4(0.10f, 0.10f, 0.14f, 1.0f);
    scheme.GroupBoxHeader = ImVec4(0.12f, 0.12f, 0.18f, 1.0f);
    scheme.GroupBoxBody = ImVec4(0.10f, 0.10f, 0.14f, 0.6f);

    // Text
    scheme.Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    scheme.TextSecondary = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    scheme.TextDisabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    // Controls
    scheme.ToggleOn = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    scheme.ToggleOff = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    scheme.SliderTrack = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    scheme.SliderFill = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    scheme.SliderGrab = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Borders
    scheme.Border = ImVec4(0.25f, 0.25f, 0.30f, 1.0f);
    scheme.BorderHover = ImVec4(0.35f, 0.35f, 0.40f, 1.0f);

    // Status
    scheme.Success = ImVec4(0.26f, 0.78f, 0.42f, 1.0f);
    scheme.Warning = ImVec4(0.98f, 0.78f, 0.26f, 1.0f);
    scheme.Error = ImVec4(0.98f, 0.36f, 0.36f, 1.0f);

    return scheme;
}

ColorScheme GetDarkPurpleScheme() {
    ColorScheme scheme;

    // Primary - Purple
    scheme.Primary = ImVec4(0.60f, 0.40f, 0.90f, 1.0f);
    scheme.PrimaryHover = ImVec4(0.70f, 0.50f, 1.0f, 1.0f);
    scheme.PrimaryActive = ImVec4(0.50f, 0.30f, 0.80f, 1.0f);

    // Accent
    scheme.Accent = ImVec4(0.75f, 0.55f, 1.0f, 1.0f);
    scheme.AccentHover = ImVec4(0.85f, 0.65f, 1.0f, 1.0f);

    // Background
    scheme.Background = ImVec4(0.09f, 0.08f, 0.12f, 1.0f);
    scheme.BackgroundAlt = ImVec4(0.11f, 0.10f, 0.15f, 1.0f);
    scheme.GroupBoxHeader = ImVec4(0.14f, 0.12f, 0.20f, 1.0f);
    scheme.GroupBoxBody = ImVec4(0.11f, 0.10f, 0.15f, 0.6f);

    // Text
    scheme.Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    scheme.TextSecondary = ImVec4(0.75f, 0.70f, 0.80f, 1.0f);
    scheme.TextDisabled = ImVec4(0.5f, 0.5f, 0.55f, 1.0f);

    // Controls
    scheme.ToggleOn = ImVec4(0.60f, 0.40f, 0.90f, 1.0f);
    scheme.ToggleOff = ImVec4(0.3f, 0.28f, 0.35f, 1.0f);
    scheme.SliderTrack = ImVec4(0.22f, 0.20f, 0.28f, 1.0f);
    scheme.SliderFill = ImVec4(0.60f, 0.40f, 0.90f, 1.0f);
    scheme.SliderGrab = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Borders
    scheme.Border = ImVec4(0.28f, 0.25f, 0.35f, 1.0f);
    scheme.BorderHover = ImVec4(0.40f, 0.35f, 0.50f, 1.0f);

    // Status
    scheme.Success = ImVec4(0.40f, 0.80f, 0.55f, 1.0f);
    scheme.Warning = ImVec4(1.0f, 0.80f, 0.30f, 1.0f);
    scheme.Error = ImVec4(1.0f, 0.40f, 0.40f, 1.0f);

    return scheme;
}

ColorScheme GetDarkGreenScheme() {
    ColorScheme scheme;

    // Primary - Green
    scheme.Primary = ImVec4(0.26f, 0.80f, 0.50f, 1.0f);
    scheme.PrimaryHover = ImVec4(0.36f, 0.90f, 0.60f, 1.0f);
    scheme.PrimaryActive = ImVec4(0.20f, 0.70f, 0.42f, 1.0f);

    // Accent
    scheme.Accent = ImVec4(0.40f, 0.90f, 0.65f, 1.0f);
    scheme.AccentHover = ImVec4(0.50f, 1.0f, 0.75f, 1.0f);

    // Background
    scheme.Background = ImVec4(0.07f, 0.10f, 0.09f, 1.0f);
    scheme.BackgroundAlt = ImVec4(0.09f, 0.12f, 0.11f, 1.0f);
    scheme.GroupBoxHeader = ImVec4(0.10f, 0.15f, 0.12f, 1.0f);
    scheme.GroupBoxBody = ImVec4(0.09f, 0.12f, 0.11f, 0.6f);

    // Text
    scheme.Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    scheme.TextSecondary = ImVec4(0.70f, 0.78f, 0.72f, 1.0f);
    scheme.TextDisabled = ImVec4(0.50f, 0.55f, 0.52f, 1.0f);

    // Controls
    scheme.ToggleOn = ImVec4(0.26f, 0.80f, 0.50f, 1.0f);
    scheme.ToggleOff = ImVec4(0.28f, 0.32f, 0.30f, 1.0f);
    scheme.SliderTrack = ImVec4(0.18f, 0.22f, 0.20f, 1.0f);
    scheme.SliderFill = ImVec4(0.26f, 0.80f, 0.50f, 1.0f);
    scheme.SliderGrab = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Borders
    scheme.Border = ImVec4(0.22f, 0.28f, 0.25f, 1.0f);
    scheme.BorderHover = ImVec4(0.32f, 0.40f, 0.35f, 1.0f);

    // Status
    scheme.Success = ImVec4(0.30f, 0.85f, 0.55f, 1.0f);
    scheme.Warning = ImVec4(0.95f, 0.80f, 0.30f, 1.0f);
    scheme.Error = ImVec4(0.95f, 0.40f, 0.40f, 1.0f);

    return scheme;
}

ColorScheme GetDarkRedScheme() {
    ColorScheme scheme;

    // Primary - Red
    scheme.Primary = ImVec4(0.90f, 0.35f, 0.35f, 1.0f);
    scheme.PrimaryHover = ImVec4(1.0f, 0.45f, 0.45f, 1.0f);
    scheme.PrimaryActive = ImVec4(0.80f, 0.28f, 0.28f, 1.0f);

    // Accent
    scheme.Accent = ImVec4(1.0f, 0.50f, 0.50f, 1.0f);
    scheme.AccentHover = ImVec4(1.0f, 0.60f, 0.60f, 1.0f);

    // Background
    scheme.Background = ImVec4(0.10f, 0.08f, 0.08f, 1.0f);
    scheme.BackgroundAlt = ImVec4(0.12f, 0.10f, 0.10f, 1.0f);
    scheme.GroupBoxHeader = ImVec4(0.16f, 0.12f, 0.12f, 1.0f);
    scheme.GroupBoxBody = ImVec4(0.12f, 0.10f, 0.10f, 0.6f);

    // Text
    scheme.Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    scheme.TextSecondary = ImVec4(0.80f, 0.72f, 0.72f, 1.0f);
    scheme.TextDisabled = ImVec4(0.55f, 0.50f, 0.50f, 1.0f);

    // Controls
    scheme.ToggleOn = ImVec4(0.90f, 0.35f, 0.35f, 1.0f);
    scheme.ToggleOff = ImVec4(0.32f, 0.28f, 0.28f, 1.0f);
    scheme.SliderTrack = ImVec4(0.22f, 0.18f, 0.18f, 1.0f);
    scheme.SliderFill = ImVec4(0.90f, 0.35f, 0.35f, 1.0f);
    scheme.SliderGrab = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Borders
    scheme.Border = ImVec4(0.30f, 0.24f, 0.24f, 1.0f);
    scheme.BorderHover = ImVec4(0.45f, 0.35f, 0.35f, 1.0f);

    // Status
    scheme.Success = ImVec4(0.40f, 0.82f, 0.50f, 1.0f);
    scheme.Warning = ImVec4(1.0f, 0.78f, 0.30f, 1.0f);
    scheme.Error = ImVec4(1.0f, 0.38f, 0.38f, 1.0f);

    return scheme;
}

//-----------------------------------------------------------------------------
// Theme Management
//-----------------------------------------------------------------------------

void SetColorScheme(const ColorScheme& scheme) {
    g_colorScheme = scheme;
    g_initialized = true;
}

void SetSizeConfig(const SizeConfig& config) {
    g_sizeConfig = config;
}

ColorScheme& GetColorScheme() {
    if (!g_initialized) {
        g_colorScheme = GetDarkBlueScheme();
        g_initialized = true;
    }
    return g_colorScheme;
}

SizeConfig& GetSizeConfig() {
    return g_sizeConfig;
}

//-----------------------------------------------------------------------------
// Animation System
//-----------------------------------------------------------------------------

void UpdateAnimations(float deltaTime) {
    g_deltaTime = deltaTime;
}

float Animate(ImGuiID id, float target, float speed) {
    auto& state = g_animations[id];

    // Smooth interpolation with damping
    float diff = target - state.current;
    state.velocity += diff * speed * g_deltaTime;
    state.velocity *= 0.85f; // Damping
    state.current += state.velocity;

    // Snap to target if very close
    if (std::abs(diff) < 0.001f && std::abs(state.velocity) < 0.001f) {
        state.current = target;
        state.velocity = 0.0f;
    }

    return state.current;
}

//-----------------------------------------------------------------------------
// Hotkey Display
//-----------------------------------------------------------------------------

static char g_hotkeyDisplayBuffer[128];

const char* GetKeyName(int vk) {
    switch (vk) {
        case 0x08: return "Backspace";
        case 0x09: return "Tab";
        case 0x0D: return "Enter";
        case 0x10: return "Shift";
        case 0x11: return "Ctrl";
        case 0x12: return "Alt";
        case 0x13: return "Pause";
        case 0x14: return "CapsLock";
        case 0x1B: return "Esc";
        case 0x20: return "Space";
        case 0x21: return "PageUp";
        case 0x22: return "PageDown";
        case 0x23: return "End";
        case 0x24: return "Home";
        case 0x25: return "Left";
        case 0x26: return "Up";
        case 0x27: return "Right";
        case 0x28: return "Down";
        case 0x2C: return "PrintScreen";
        case 0x2D: return "Insert";
        case 0x2E: return "Delete";

        // Numbers
        case 0x30: return "0";
        case 0x31: return "1";
        case 0x32: return "2";
        case 0x33: return "3";
        case 0x34: return "4";
        case 0x35: return "5";
        case 0x36: return "6";
        case 0x37: return "7";
        case 0x38: return "8";
        case 0x39: return "9";

        // Letters
        case 0x41: return "A";
        case 0x42: return "B";
        case 0x43: return "C";
        case 0x44: return "D";
        case 0x45: return "E";
        case 0x46: return "F";
        case 0x47: return "G";
        case 0x48: return "H";
        case 0x49: return "I";
        case 0x4A: return "J";
        case 0x4B: return "K";
        case 0x4C: return "L";
        case 0x4D: return "M";
        case 0x4E: return "N";
        case 0x4F: return "O";
        case 0x50: return "P";
        case 0x51: return "Q";
        case 0x52: return "R";
        case 0x53: return "S";
        case 0x54: return "T";
        case 0x55: return "U";
        case 0x56: return "V";
        case 0x57: return "W";
        case 0x58: return "X";
        case 0x59: return "Y";
        case 0x5A: return "Z";

        // Numpad
        case 0x60: return "Num0";
        case 0x61: return "Num1";
        case 0x62: return "Num2";
        case 0x63: return "Num3";
        case 0x64: return "Num4";
        case 0x65: return "Num5";
        case 0x66: return "Num6";
        case 0x67: return "Num7";
        case 0x68: return "Num8";
        case 0x69: return "Num9";
        case 0x6A: return "Num*";
        case 0x6B: return "Num+";
        case 0x6D: return "Num-";
        case 0x6E: return "Num.";
        case 0x6F: return "Num/";

        // Function keys
        case 0x70: return "F1";
        case 0x71: return "F2";
        case 0x72: return "F3";
        case 0x73: return "F4";
        case 0x74: return "F5";
        case 0x75: return "F6";
        case 0x76: return "F7";
        case 0x77: return "F8";
        case 0x78: return "F9";
        case 0x79: return "F10";
        case 0x7A: return "F11";
        case 0x7B: return "F12";

        // Others
        case 0x90: return "NumLock";
        case 0x91: return "ScrollLock";
        case 0xA0: return "LShift";
        case 0xA1: return "RShift";
        case 0xA2: return "LCtrl";
        case 0xA3: return "RCtrl";
        case 0xA4: return "LAlt";
        case 0xA5: return "RAlt";

        default: return "Unknown";
    }
}

const char* GetHotkeyDisplayString(const HotkeyBinding& binding) {
    if (!binding.IsValid()) {
        snprintf(g_hotkeyDisplayBuffer, sizeof(g_hotkeyDisplayBuffer), "None");
        return g_hotkeyDisplayBuffer;
    }

    g_hotkeyDisplayBuffer[0] = '\0';
    if (binding.ctrl) strcat(g_hotkeyDisplayBuffer, "Ctrl+");
    if (binding.alt) strcat(g_hotkeyDisplayBuffer, "Alt+");
    if (binding.shift) strcat(g_hotkeyDisplayBuffer, "Shift+");
    strcat(g_hotkeyDisplayBuffer, GetKeyName(binding.key));

    return g_hotkeyDisplayBuffer;
}

} // namespace StyleUI
