#pragma once

#include <imgui.h>

namespace Theme {

// Color palette
namespace Colors {
    // Primary colors
    inline ImVec4 Primary = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);       // Blue
    inline ImVec4 PrimaryHover = ImVec4(0.36f, 0.69f, 1.0f, 1.0f);
    inline ImVec4 PrimaryActive = ImVec4(0.16f, 0.49f, 0.88f, 1.0f);

    // Background colors
    inline ImVec4 Background = ImVec4(0.06f, 0.06f, 0.09f, 1.0f);    // Dark background
    inline ImVec4 Panel = ImVec4(0.12f, 0.12f, 0.16f, 0.95f);        // Panel background
    inline ImVec4 PanelHover = ImVec4(0.16f, 0.16f, 0.20f, 0.95f);

    // Text colors
    inline ImVec4 Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    inline ImVec4 TextDisabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    inline ImVec4 TextHint = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

    // Input colors
    inline ImVec4 Input = ImVec4(0.08f, 0.08f, 0.12f, 1.0f);
    inline ImVec4 InputHover = ImVec4(0.12f, 0.12f, 0.16f, 1.0f);
    inline ImVec4 InputFocus = ImVec4(0.14f, 0.14f, 0.18f, 1.0f);

    // Tab colors
    inline ImVec4 Tab = ImVec4(0.16f, 0.16f, 0.20f, 1.0f);
    inline ImVec4 TabHover = ImVec4(0.20f, 0.20f, 0.26f, 1.0f);
    inline ImVec4 TabActive = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);

    // Status colors
    inline ImVec4 Success = ImVec4(0.26f, 0.78f, 0.42f, 1.0f);       // Green
    inline ImVec4 Warning = ImVec4(0.98f, 0.78f, 0.26f, 1.0f);       // Yellow/Orange
    inline ImVec4 Error = ImVec4(0.98f, 0.36f, 0.36f, 1.0f);         // Red
    inline ImVec4 Info = ImVec4(0.70f, 0.80f, 0.95f, 1.0f);          // Light Blue (neutral)

    // Border
    inline ImVec4 Border = ImVec4(0.26f, 0.26f, 0.32f, 1.0f);
    inline ImVec4 BorderHover = ImVec4(0.36f, 0.36f, 0.42f, 1.0f);
}

// Dimensions
namespace Size {
    inline float Rounding = 8.0f;
    inline float BorderWidth = 1.0f;
    inline float ItemSpacing = 8.0f;
    inline float FramePadding = 12.0f;
    inline float ButtonHeight = 40.0f;
    inline float InputHeight = 42.0f;
    inline float TabHeight = 36.0f;
}

// Apply theme to ImGui
void Apply();

// Get gradient colors for buttons (top, bottom)
ImU32 GetGradientTop(const ImVec4& base);
ImU32 GetGradientBottom(const ImVec4& base);

} // namespace Theme
