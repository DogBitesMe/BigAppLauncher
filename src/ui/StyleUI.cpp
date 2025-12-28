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

// Glass effect state
static ID3D11ShaderResourceView* g_blurredSRV = nullptr;
static int g_screenWidth = 0;
static int g_screenHeight = 0;

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
    scheme.TabActiveText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Pure white for active tab

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
    scheme.TabActiveText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Pure white for active tab

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
    scheme.TabActiveText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Pure white for active tab

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
    scheme.TabActiveText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Pure white for active tab

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
    // Check if this is first-time access
    auto it = g_animations.find(id);
    if (it == g_animations.end()) {
        // First time - initialize directly to target (no animation on first display)
        g_animations[id] = AnimState{target, 0.0f};
        return target;
    }

    auto& state = it->second;

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

float AnimateLinear(ImGuiID id, float target, float speed) {
    // Check if this is first-time access
    auto it = g_animations.find(id);
    if (it == g_animations.end()) {
        // First time - initialize directly to target (no animation on first display)
        g_animations[id] = AnimState{target, 0.0f};
        return target;
    }

    auto& state = it->second;

    float diff = target - state.current;
    float step = speed * g_deltaTime;

    // Linear interpolation - no oscillation
    if (std::abs(diff) <= step) {
        state.current = target;
    } else {
        state.current += (diff > 0 ? step : -step);
    }

    state.velocity = 0.0f; // Not used in linear mode
    return state.current;
}

//-----------------------------------------------------------------------------
// Glass Effect System
//-----------------------------------------------------------------------------

void SetBlurredBackgroundSRV(ID3D11ShaderResourceView* srv, int screenWidth, int screenHeight) {
    g_blurredSRV = srv;
    g_screenWidth = screenWidth;
    g_screenHeight = screenHeight;
}

ID3D11ShaderResourceView* GetBlurredBackgroundSRV() {
    return g_blurredSRV;
}

bool IsGlassAvailable() {
    return g_blurredSRV != nullptr && g_screenWidth > 0 && g_screenHeight > 0;
}

//-----------------------------------------------------------------------------
// Hotkey Display
//-----------------------------------------------------------------------------

static char g_hotkeyDisplayBuffer[128];

const char* GetKeyName(int vk) {
    switch (vk) {
        // Mouse buttons
        case 0x01: return "Mouse1";      // VK_LBUTTON
        case 0x02: return "Mouse2";      // VK_RBUTTON
        case 0x04: return "Mouse3";      // VK_MBUTTON
        case 0x05: return "Mouse4";      // VK_XBUTTON1
        case 0x06: return "Mouse5";      // VK_XBUTTON2

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

        // OEM keys (standard US keyboard layout)
        case 0xBA: return ";";          // VK_OEM_1 - semicolon
        case 0xBB: return "=";          // VK_OEM_PLUS - equals/plus
        case 0xBC: return ",";          // VK_OEM_COMMA
        case 0xBD: return "-";          // VK_OEM_MINUS
        case 0xBE: return ".";          // VK_OEM_PERIOD
        case 0xBF: return "/";          // VK_OEM_2 - slash
        case 0xC0: return "`";          // VK_OEM_3 - grave/tilde
        case 0xDB: return "[";          // VK_OEM_4 - open bracket
        case 0xDC: return "\\";         // VK_OEM_5 - backslash
        case 0xDD: return "]";          // VK_OEM_6 - close bracket
        case 0xDE: return "'";          // VK_OEM_7 - apostrophe

        // Extended function keys
        case 0x7C: return "F13";
        case 0x7D: return "F14";
        case 0x7E: return "F15";
        case 0x7F: return "F16";
        case 0x80: return "F17";
        case 0x81: return "F18";
        case 0x82: return "F19";
        case 0x83: return "F20";
        case 0x84: return "F21";
        case 0x85: return "F22";
        case 0x86: return "F23";
        case 0x87: return "F24";

        // Additional system keys
        case 0x03: return "Cancel";     // VK_CANCEL (Ctrl+Break)
        case 0x0C: return "Clear";      // VK_CLEAR
        case 0x15: return "IME Kana";   // VK_KANA
        case 0x19: return "IME Kanji";  // VK_KANJI
        case 0x1C: return "IME Convert"; // VK_CONVERT
        case 0x2F: return "Help";       // VK_HELP
        case 0x5B: return "LWin";       // VK_LWIN
        case 0x5C: return "RWin";       // VK_RWIN
        case 0x5D: return "Apps";       // VK_APPS (context menu)
        case 0x5F: return "Sleep";      // VK_SLEEP
        case 0x6C: return "Separator";  // VK_SEPARATOR (numpad separator)

        // Browser keys
        case 0xA6: return "BrowserBack";
        case 0xA7: return "BrowserFwd";
        case 0xA8: return "BrowserRefresh";
        case 0xA9: return "BrowserStop";
        case 0xAA: return "BrowserSearch";
        case 0xAB: return "BrowserFav";
        case 0xAC: return "BrowserHome";

        // Media keys
        case 0xAD: return "VolMute";
        case 0xAE: return "VolDown";
        case 0xAF: return "VolUp";
        case 0xB0: return "MediaNext";
        case 0xB1: return "MediaPrev";
        case 0xB2: return "MediaStop";
        case 0xB3: return "MediaPlay";
        case 0xB4: return "LaunchMail";
        case 0xB5: return "LaunchMedia";
        case 0xB6: return "LaunchApp1";
        case 0xB7: return "LaunchApp2";

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
