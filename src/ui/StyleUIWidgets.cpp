#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "StyleUI.h"
#include <imgui_internal.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include "../i18n/Localization.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace StyleUI {

//-----------------------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------------------

static ImU32 ColorToU32(const ImVec4& col) {
    return ImGui::ColorConvertFloat4ToU32(col);
}

static ImVec4 LerpColor(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

static ImU32 GetGradientColor(const ImVec4& top, const ImVec4& bottom, float t) {
    return ColorToU32(LerpColor(top, bottom, t));
}

//-----------------------------------------------------------------------------
// Glass Effect Drawing
//-----------------------------------------------------------------------------

void DrawGlassRect(const ImVec2& pos, const ImVec2& size, const GlassConfig& config) {
    if (!IsGlassAvailable()) {
        // Fallback: draw solid background if glass not available
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                         config.tintColor, config.rounding);
        if (config.drawBorder) {
            dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                       config.borderColor, config.rounding, 0, config.borderThickness);
        }
        return;
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    // Calculate UV coordinates based on screen position
    ImVec2 uv0(pos.x / screenSize.x, pos.y / screenSize.y);
    ImVec2 uv1((pos.x + size.x) / screenSize.x, (pos.y + size.y) / screenSize.y);

    // Clamp UVs to valid range
    uv0.x = (uv0.x < 0) ? 0 : (uv0.x > 1 ? 1 : uv0.x);
    uv0.y = (uv0.y < 0) ? 0 : (uv0.y > 1 ? 1 : uv0.y);
    uv1.x = (uv1.x < 0) ? 0 : (uv1.x > 1 ? 1 : uv1.x);
    uv1.y = (uv1.y < 0) ? 0 : (uv1.y > 1 ? 1 : uv1.y);

    // Draw blurred background with alpha
    ImU32 blurColor = IM_COL32(255, 255, 255, (int)(255 * config.alpha));
    dl->AddImageRounded(
        (ImTextureID)GetBlurredBackgroundSRV(),
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        uv0, uv1,
        blurColor,
        config.rounding
    );

    // Draw tint overlay for frosted glass effect
    dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                     config.tintColor, config.rounding);

    // Draw subtle border glow
    if (config.drawBorder) {
        dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                   config.borderColor, config.rounding, 0, config.borderThickness);
    }
}

void DrawGlassRectSimple(const ImVec2& pos, const ImVec2& size, float alpha, float rounding) {
    GlassConfig config;
    config.alpha = alpha;
    config.rounding = rounding;
    DrawGlassRect(pos, size, config);
}

//-----------------------------------------------------------------------------
// GroupBox
//-----------------------------------------------------------------------------

static int g_groupBoxDepth = 0;
static ImVec2 g_groupBoxStartPos;

bool BeginGroupBox(const char* label, const ImVec2& size) {
    return BeginGroupBoxEx(nullptr, label, size);
}

bool BeginGroupBoxEx(const char* icon, const char* label, const ImVec2& size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    g_groupBoxStartPos = window->DC.CursorPos;

    // Calculate actual size
    ImVec2 contentSize = size;
    if (contentSize.x <= 0) contentSize.x = ImGui::GetContentRegionAvail().x;
    if (contentSize.y <= 0) contentSize.y = 0; // Will auto-resize

    // Store for EndGroupBox
    g_groupBoxDepth++;

    ImGui::PushID(id);

    // Begin child with custom styling
    ImGui::PushStyleColor(ImGuiCol_ChildBg, colors.GroupBoxBody);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, sizes.Rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(sizes.FramePadding, sizes.FramePadding));

    bool opened = ImGui::BeginChild(label, contentSize, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

    // Draw header background
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    float windowWidth = ImGui::GetWindowSize().x;

    // Header bar
    drawList->AddRectFilled(
        windowPos,
        ImVec2(windowPos.x + windowWidth, windowPos.y + sizes.GroupBoxHeaderHeight),
        ColorToU32(colors.GroupBoxHeader),
        sizes.Rounding,
        ImDrawFlags_RoundCornersTop
    );

    // Header text
    float textY = windowPos.y + (sizes.GroupBoxHeaderHeight - ImGui::GetFontSize()) * 0.5f;

    if (icon && icon[0]) {
        // Icon + Label
        char fullLabel[256];
        snprintf(fullLabel, sizeof(fullLabel), "%s  %s", icon, label);
        drawList->AddText(
            ImVec2(windowPos.x + sizes.FramePadding, textY),
            ColorToU32(colors.Text),
            fullLabel
        );
    } else {
        drawList->AddText(
            ImVec2(windowPos.x + sizes.FramePadding, textY),
            ColorToU32(colors.Text),
            label
        );
    }

    // Move cursor below header
    ImGui::SetCursorPosY(sizes.GroupBoxHeaderHeight + sizes.FramePadding);

    return opened;
}

void EndGroupBox() {
    if (g_groupBoxDepth <= 0) return;
    g_groupBoxDepth--;

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    ImGui::PopID();

    ImGui::Spacing();
}

//-----------------------------------------------------------------------------
// Glass GroupBox
//-----------------------------------------------------------------------------

static int g_glassGroupBoxDepth = 0;
static GlassConfig g_currentGlassConfig;

bool BeginGroupBoxGlass(const char* label, const ImVec2& size, const GlassConfig& config) {
    return BeginGroupBoxExGlass(nullptr, label, size, config);
}

bool BeginGroupBoxExGlass(const char* icon, const char* label, const ImVec2& size, const GlassConfig& config) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    // Calculate actual size
    ImVec2 contentSize = size;
    if (contentSize.x <= 0) contentSize.x = ImGui::GetContentRegionAvail().x;
    if (contentSize.y <= 0) contentSize.y = 0; // Will auto-resize

    // Store for EndGroupBoxGlass
    g_glassGroupBoxDepth++;
    g_currentGlassConfig = config;

    ImGui::PushID(id);

    // Begin child with transparent background (glass effect will be drawn separately)
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0)); // Transparent
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, config.rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(sizes.FramePadding, sizes.FramePadding));

    bool opened = ImGui::BeginChild(label, contentSize, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

    // Get actual window position and size for glass effect
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    // Draw the glass effect
    GlassConfig glassConfig = config;
    glassConfig.rounding = sizes.Rounding;
    DrawGlassRect(windowPos, windowSize, glassConfig);

    // Draw header background (slightly more opaque)
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float windowWidth = ImGui::GetWindowSize().x;

    // Header bar (semi-transparent overlay on top of glass)
    drawList->AddRectFilled(
        windowPos,
        ImVec2(windowPos.x + windowWidth, windowPos.y + sizes.GroupBoxHeaderHeight),
        IM_COL32(0, 0, 0, 80), // Subtle dark overlay for header
        sizes.Rounding,
        ImDrawFlags_RoundCornersTop
    );

    // Header text
    float textY = windowPos.y + (sizes.GroupBoxHeaderHeight - ImGui::GetFontSize()) * 0.5f;

    if (icon && icon[0]) {
        // Icon + Label
        char fullLabel[256];
        snprintf(fullLabel, sizeof(fullLabel), "%s  %s", icon, label);
        drawList->AddText(
            ImVec2(windowPos.x + sizes.FramePadding, textY),
            ColorToU32(colors.Text),
            fullLabel
        );
    } else {
        drawList->AddText(
            ImVec2(windowPos.x + sizes.FramePadding, textY),
            ColorToU32(colors.Text),
            label
        );
    }

    // Move cursor below header
    ImGui::SetCursorPosY(sizes.GroupBoxHeaderHeight + sizes.FramePadding);

    return opened;
}

void EndGroupBoxGlass() {
    if (g_glassGroupBoxDepth <= 0) return;
    g_glassGroupBoxDepth--;

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    ImGui::PopID();

    ImGui::Spacing();
}

//-----------------------------------------------------------------------------
// Toggle Switch
//-----------------------------------------------------------------------------

bool ToggleSwitch(const char* label, bool* v) {
    const auto& colors = GetColorScheme();
    return ToggleSwitchEx(label, v, colors.ToggleOn, colors.ToggleOff);
}

bool ToggleSwitchEx(const char* label, bool* v, const ImVec4& onColor, const ImVec4& offColor) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float toggleW = sizes.ToggleWidth;
    float toggleH = sizes.ToggleHeight;
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float totalW = ImGui::GetContentRegionAvail().x;
    float rowHeight = std::max(toggleH, labelSize.y);

    // Bounding box for entire row
    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowHeight));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    // Interaction
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) *v = !*v;

    ImDrawList* dl = window->DrawList;

    // Draw label (left side)
    float textY = pos.y + (rowHeight - labelSize.y) * 0.5f;
    dl->AddText(
        ImVec2(pos.x, textY),
        ColorToU32(colors.Text),
        label
    );

    // Animate toggle position
    float animT = Animate(id, *v ? 1.0f : 0.0f, 15.0f);

    // Toggle position (right side)
    float toggleX = pos.x + totalW - toggleW;
    float toggleY = pos.y + (rowHeight - toggleH) * 0.5f;

    // Lerp track color
    ImVec4 trackColor = LerpColor(offColor, onColor, animT);

    // Track background
    dl->AddRectFilled(
        ImVec2(toggleX, toggleY),
        ImVec2(toggleX + toggleW, toggleY + toggleH),
        ColorToU32(trackColor),
        toggleH * 0.5f
    );

    // Knob
    float knobRadius = (toggleH - 4.0f) * 0.5f;
    float knobMinX = toggleX + knobRadius + 2.0f;
    float knobMaxX = toggleX + toggleW - knobRadius - 2.0f;
    float knobX = knobMinX + (knobMaxX - knobMinX) * animT;
    float knobY = toggleY + toggleH * 0.5f;

    // Knob shadow
    dl->AddCircleFilled(
        ImVec2(knobX + 1, knobY + 1),
        knobRadius,
        IM_COL32(0, 0, 0, 40)
    );

    // Knob
    dl->AddCircleFilled(
        ImVec2(knobX, knobY),
        knobRadius,
        IM_COL32(255, 255, 255, 255)
    );

    return pressed;
}

//-----------------------------------------------------------------------------
// Checkbox
//-----------------------------------------------------------------------------

bool Checkbox(const char* label, bool* v) {
    // Text left, checkbox right
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float boxSize = sizes.CheckboxSize;
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float totalW = ImGui::GetContentRegionAvail().x;
    float rowHeight = std::max(boxSize, labelSize.y);

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowHeight));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    // Use IsMouseClicked for single-click behavior (not continuous)
    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    if (clicked) {
        *v = !*v;
    }

    ImDrawList* dl = window->DrawList;

    // Label (left)
    float textY = pos.y + (rowHeight - labelSize.y) * 0.5f;
    dl->AddText(
        ImVec2(pos.x, textY),
        ColorToU32(colors.Text),
        label
    );

    // Checkbox (right)
    float boxX = pos.x + totalW - boxSize;
    float boxY = pos.y + (rowHeight - boxSize) * 0.5f;
    ImVec2 boxMin(boxX, boxY);
    ImVec2 boxMax(boxX + boxSize, boxY + boxSize);

    // Animate check based on current value (use linear for smooth fade without oscillation)
    float animT = AnimateLinear(id, *v ? 1.0f : 0.0f, 15.0f);

    // Background color
    ImVec4 bgColor = LerpColor(hovered ? colors.BorderHover : colors.Border, colors.Primary, animT);
    dl->AddRectFilled(boxMin, boxMax, ColorToU32(bgColor), sizes.Rounding * 0.5f);

    // Checkmark
    if (animT > 0.01f) {
        float pad = boxSize * 0.2f;
        float checkX = boxX + pad;
        float checkY = boxY + boxSize * 0.5f;

        ImVec2 p1(checkX, checkY);
        ImVec2 p2(checkX + boxSize * 0.25f, checkY + boxSize * 0.25f);
        ImVec2 p3(checkX + boxSize - pad * 2, checkY - boxSize * 0.2f);

        ImU32 checkColor = IM_COL32(255, 255, 255, (int)(255 * animT));
        dl->AddLine(p1, p2, checkColor, 2.0f);
        dl->AddLine(p2, p3, checkColor, 2.0f);
    }

    return clicked;
}

bool CheckboxClassic(const char* label, bool* v) {
    // Checkbox left, text right (classic style)
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float boxSize = sizes.CheckboxSize;
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float totalW = boxSize + sizes.ItemSpacing + labelSize.x;
    float rowHeight = std::max(boxSize, labelSize.y);

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowHeight));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    // Use IsMouseClicked for single-click behavior (not continuous)
    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    if (clicked) {
        *v = !*v;
    }

    ImDrawList* dl = window->DrawList;

    // Checkbox (left)
    float boxY = pos.y + (rowHeight - boxSize) * 0.5f;
    ImVec2 boxMin(pos.x, boxY);
    ImVec2 boxMax(pos.x + boxSize, boxY + boxSize);

    // Use linear animation for smooth fade without oscillation
    float animT = AnimateLinear(id, *v ? 1.0f : 0.0f, 15.0f);

    ImVec4 bgColor = LerpColor(hovered ? colors.BorderHover : colors.Border, colors.Primary, animT);
    dl->AddRectFilled(boxMin, boxMax, ColorToU32(bgColor), sizes.Rounding * 0.5f);

    // Checkmark
    if (animT > 0.01f) {
        float pad = boxSize * 0.2f;
        float checkX = pos.x + pad;
        float checkY = boxY + boxSize * 0.5f;

        ImVec2 p1(checkX, checkY);
        ImVec2 p2(checkX + boxSize * 0.25f, checkY + boxSize * 0.25f);
        ImVec2 p3(checkX + boxSize - pad * 2, checkY - boxSize * 0.2f);

        ImU32 checkColor = IM_COL32(255, 255, 255, (int)(255 * animT));
        dl->AddLine(p1, p2, checkColor, 2.0f);
        dl->AddLine(p2, p3, checkColor, 2.0f);
    }

    // Label (right)
    float textY = pos.y + (rowHeight - labelSize.y) * 0.5f;
    dl->AddText(
        ImVec2(pos.x + boxSize + sizes.ItemSpacing, textY),
        ColorToU32(colors.Text),
        label
    );

    return clicked;
}

//-----------------------------------------------------------------------------
// Sliders
//-----------------------------------------------------------------------------

bool SliderFloat(const char* label, float* v, float min, float max, const char* format) {
    const auto& colors = GetColorScheme();
    return SliderFloatGradient(label, v, min, max, colors.SliderTrack, colors.SliderFill, format);
}

bool SliderInt(const char* label, int* v, int min, int max, const char* format) {
    float fv = (float)*v;
    bool changed = SliderFloat(label, &fv, (float)min, (float)max, format);
    if (changed) *v = (int)fv;
    return changed;
}

bool SliderFloatGradient(const char* label, float* v, float min, float max,
                         const ImVec4& trackColor, const ImVec4& fillColor, const char* format) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float totalW = ImGui::GetContentRegionAvail().x;
    float sliderH = sizes.SliderHeight;
    float grabW = sizes.SliderGrabWidth;

    // Layout: Label on top, slider below
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float totalH = labelSize.y + sizes.ItemSpacing + sliderH;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + totalH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;

    // Draw label with value
    char valueText[64];
    snprintf(valueText, sizeof(valueText), format, *v);

    dl->AddText(
        ImVec2(pos.x, pos.y),
        ColorToU32(colors.Text),
        label
    );

    ImVec2 valueSize = ImGui::CalcTextSize(valueText);
    dl->AddText(
        ImVec2(pos.x + totalW - valueSize.x, pos.y),
        ColorToU32(colors.TextSecondary),
        valueText
    );

    // Slider track position
    float trackY = pos.y + labelSize.y + sizes.ItemSpacing;
    float trackH = sliderH * 0.4f;
    float trackYCenter = trackY + (sliderH - trackH) * 0.5f;

    ImRect sliderBB(ImVec2(pos.x, trackY), ImVec2(pos.x + totalW, trackY + sliderH));

    // Interaction
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(sliderBB, id, &hovered, &held);

    if (held) {
        float mouseX = ImGui::GetIO().MousePos.x;
        float t = (mouseX - pos.x - grabW * 0.5f) / (totalW - grabW);
        t = ImClamp(t, 0.0f, 1.0f);
        *v = min + (max - min) * t;
    }

    // Normalize value
    float t = (*v - min) / (max - min);
    t = ImClamp(t, 0.0f, 1.0f);

    // Draw track background
    dl->AddRectFilled(
        ImVec2(pos.x, trackYCenter),
        ImVec2(pos.x + totalW, trackYCenter + trackH),
        ColorToU32(trackColor),
        trackH * 0.5f
    );

    // Draw filled portion with stronger gradient
    float fillWidth = (totalW - grabW) * t;
    if (fillWidth > 0) {
        // Create gradient effect with stronger contrast (1.8x brightness)
        ImVec2 fillMin(pos.x, trackYCenter);
        ImVec2 fillMax(pos.x + fillWidth + grabW * 0.5f, trackYCenter + trackH);

        // Darker left side (0.7x)
        ImU32 gradLeft = ColorToU32(ImVec4(
            fillColor.x * 0.7f,
            fillColor.y * 0.7f,
            fillColor.z * 0.7f,
            fillColor.w
        ));
        // Brighter right side (1.8x, clamped to 1.0)
        ImU32 gradRight = ColorToU32(ImVec4(
            std::min(fillColor.x * 1.8f, 1.0f),
            std::min(fillColor.y * 1.8f, 1.0f),
            std::min(fillColor.z * 1.8f, 1.0f),
            fillColor.w
        ));

        dl->AddRectFilledMultiColor(fillMin, fillMax, gradLeft, gradRight, gradRight, gradLeft);
    }

    // Draw grab handle
    float grabX = pos.x + (totalW - grabW) * t;
    float grabY = trackY + (sliderH - grabW) * 0.5f;

    // Grab shadow
    dl->AddCircleFilled(
        ImVec2(grabX + grabW * 0.5f + 1, grabY + grabW * 0.5f + 1),
        grabW * 0.5f,
        IM_COL32(0, 0, 0, 40)
    );

    // Grab handle
    ImU32 grabColor = held ? ColorToU32(colors.PrimaryActive) :
                     (hovered ? ColorToU32(colors.PrimaryHover) : ColorToU32(colors.SliderGrab));
    dl->AddCircleFilled(
        ImVec2(grabX + grabW * 0.5f, grabY + grabW * 0.5f),
        grabW * 0.5f,
        grabColor
    );

    // Grab outline
    dl->AddCircle(
        ImVec2(grabX + grabW * 0.5f, grabY + grabW * 0.5f),
        grabW * 0.5f,
        ColorToU32(colors.Primary),
        0,
        1.5f
    );

    return held;
}

//-----------------------------------------------------------------------------
// Tab Systems
//-----------------------------------------------------------------------------

int TabBarLarge(const char* id, const char** icons, const char** labels, int count, int current) {
    TabStyle defaultStyle;
    return TabBarLargeEx(id, icons, labels, count, current, defaultStyle);
}

int TabBarLargeEx(const char* id, const char** icons, const char** labels, int count, int current, const TabStyle& style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return current;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID tabId = window->GetID(id);
    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;
    float tabH = sizes.TabLargeHeight;
    float tabW = totalW / count;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + tabH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, tabId)) return current;

    ImDrawList* dl = window->DrawList;

    // Background
    dl->AddRectFilled(pos, ImVec2(pos.x + totalW, pos.y + tabH), ColorToU32(colors.BackgroundAlt), sizes.Rounding);

    int result = current;

    for (int i = 0; i < count; i++) {
        ImVec2 tabPos(pos.x + i * tabW, pos.y);
        ImRect tabBB(tabPos, ImVec2(tabPos.x + tabW, tabPos.y + tabH));

        ImGuiID itemId = tabId + i + 1;
        (void)itemId; // Unused but kept for consistency
        bool isActive = (i == current);
        bool hovered = ImGui::IsMouseHoveringRect(tabBB.Min, tabBB.Max);

        // Tab background for active tab
        if (isActive) {
            if (style.useGradient) {
                // Gradient background matching slider/button style (0.7x to 1.8x)
                ImVec4 baseColor = ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f);
                ImU32 gradLeft = ColorToU32(ImVec4(
                    baseColor.x * 0.7f, baseColor.y * 0.7f, baseColor.z * 0.7f, baseColor.w
                ));
                ImU32 gradRight = ColorToU32(ImVec4(
                    std::min(baseColor.x * 1.8f, 1.0f),
                    std::min(baseColor.y * 1.8f, 1.0f),
                    std::min(baseColor.z * 1.8f, 1.0f),
                    baseColor.w
                ));
                dl->AddRectFilledMultiColor(tabBB.Min, tabBB.Max, gradLeft, gradRight, gradRight, gradLeft);
            } else {
                // Solid color background (like SubTab Pill style)
                dl->AddRectFilled(tabBB.Min, tabBB.Max,
                    ColorToU32(ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f)),
                    sizes.Rounding);
            }
        } else if (hovered) {
            dl->AddRectFilled(tabBB.Min, tabBB.Max, IM_COL32(255, 255, 255, 10), sizes.Rounding);
        }

        // Active indicator with configurable style
        if (isActive) {
            float indicatorW = (tabW - style.indicatorPadding * 2) * style.indicatorWidthRatio;
            float indicatorX;
            if (style.indicatorCentered) {
                indicatorX = tabPos.x + (tabW - indicatorW) * 0.5f;
            } else {
                indicatorX = tabPos.x + style.indicatorPadding;
            }

            float indicatorY = tabBB.Max.y - style.indicatorHeight;

            if (style.gradientIndicator) {
                // Gradient indicator
                ImU32 gradStart = ColorToU32(style.indicatorGradientStart);
                ImU32 gradEnd = ColorToU32(style.indicatorGradientEnd);
                dl->AddRectFilledMultiColor(
                    ImVec2(indicatorX, indicatorY),
                    ImVec2(indicatorX + indicatorW, tabBB.Max.y),
                    gradStart, gradEnd, gradEnd, gradStart
                );
            } else {
                // Solid color indicator
                dl->AddRectFilled(
                    ImVec2(indicatorX, indicatorY),
                    ImVec2(indicatorX + indicatorW, tabBB.Max.y),
                    ColorToU32(colors.Primary),
                    1.5f
                );
            }
        }

        // Icon
        if (icons && icons[i]) {
            ImVec2 iconSize = ImGui::CalcTextSize(icons[i]);
            float iconX = tabPos.x + (tabW - iconSize.x) * 0.5f;
            float iconY = tabPos.y + 10;

            ImU32 iconColor = isActive ? ColorToU32(colors.Primary) : ColorToU32(colors.TextSecondary);
            dl->AddText(ImVec2(iconX, iconY), iconColor, icons[i]);
        }

        // Label
        if (labels && labels[i]) {
            ImVec2 labelSize = ImGui::CalcTextSize(labels[i]);
            float labelX = tabPos.x + (tabW - labelSize.x) * 0.5f;
            float labelY = tabPos.y + tabH - labelSize.y - 8;

            ImU32 textColor = isActive ? ColorToU32(colors.Text) : ColorToU32(colors.TextSecondary);
            dl->AddText(ImVec2(labelX, labelY), textColor, labels[i]);
        }

        // Handle click
        if (hovered && ImGui::IsMouseClicked(0)) {
            result = i;
        }
    }

    return result;
}

int TabBarSmall(const char* id, const char** labels, int count, int current) {
    return TabBarSmallIcon(id, nullptr, labels, count, current);
}

int TabBarSmallIcon(const char* id, const char** icons, const char** labels, int count, int current) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return current;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID tabId = window->GetID(id);
    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;
    float tabH = sizes.TabSmallHeight;
    float tabW = totalW / count;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + tabH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, tabId)) return current;

    ImDrawList* dl = window->DrawList;

    // Background
    dl->AddRectFilled(pos, ImVec2(pos.x + totalW, pos.y + tabH), ColorToU32(colors.BackgroundAlt), sizes.Rounding);

    int result = current;

    for (int i = 0; i < count; i++) {
        ImVec2 tabPos(pos.x + i * tabW, pos.y);
        ImRect tabBB(tabPos, ImVec2(tabPos.x + tabW, tabPos.y + tabH));

        bool isActive = (i == current);
        bool hovered = ImGui::IsMouseHoveringRect(tabBB.Min, tabBB.Max);

        // Tab background with gradient for active tab
        if (isActive) {
            // Gradient background matching slider/button style (0.7x to 1.8x)
            ImVec4 baseColor = ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f);
            ImU32 gradLeft = ColorToU32(ImVec4(
                baseColor.x * 0.7f, baseColor.y * 0.7f, baseColor.z * 0.7f, baseColor.w
            ));
            ImU32 gradRight = ColorToU32(ImVec4(
                std::min(baseColor.x * 1.8f, 1.0f),
                std::min(baseColor.y * 1.8f, 1.0f),
                std::min(baseColor.z * 1.8f, 1.0f),
                baseColor.w
            ));
            dl->AddRectFilledMultiColor(tabBB.Min, tabBB.Max, gradLeft, gradRight, gradRight, gradLeft);
        } else if (hovered) {
            dl->AddRectFilled(tabBB.Min, tabBB.Max, IM_COL32(255, 255, 255, 10), sizes.Rounding);
        }

        // Underline indicator
        if (isActive) {
            dl->AddRectFilled(
                ImVec2(tabBB.Min.x + 2, tabBB.Max.y - 2),
                ImVec2(tabBB.Max.x - 2, tabBB.Max.y),
                ColorToU32(colors.Primary)
            );
        }

        // Build label text
        char fullText[128];
        if (icons && icons[i] && labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
        } else if (labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s", labels[i]);
        } else {
            fullText[0] = '\0';
        }

        ImVec2 textSize = ImGui::CalcTextSize(fullText);
        float textX = tabPos.x + (tabW - textSize.x) * 0.5f;
        float textY = tabPos.y + (tabH - textSize.y) * 0.5f;

        ImU32 textColor = isActive ? ColorToU32(colors.Primary) : ColorToU32(colors.TextSecondary);
        dl->AddText(ImVec2(textX, textY), textColor, fullText);

        if (hovered && ImGui::IsMouseClicked(0)) {
            result = i;
        }
    }

    return result;
}

int TabBarSmallEx(const char* id, const char** labels, int count, int current, const SmallTabStyle& style) {
    return TabBarSmallIconEx(id, nullptr, labels, count, current, style);
}

int TabBarSmallIconEx(const char* id, const char** icons, const char** labels, int count, int current, const SmallTabStyle& style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return current;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID tabId = window->GetID(id);
    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;
    float tabH = sizes.TabSmallHeight;

    ImDrawList* dl = window->DrawList;
    int result = current;

    if (style.allowWrap) {
        // Calculate tab widths and determine row assignments
        std::vector<float> tabWidths(count);
        std::vector<int> tabRows(count);
        float currentRowWidth = 0;
        int currentRow = 0;

        for (int i = 0; i < count; i++) {
            // Build label text
            char fullText[128];
            if (icons && icons[i] && labels && labels[i]) {
                snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
            } else if (labels && labels[i]) {
                snprintf(fullText, sizeof(fullText), "%s", labels[i]);
            } else {
                fullText[0] = '\0';
            }

            ImVec2 textSize = ImGui::CalcTextSize(fullText);
            float tabW = textSize.x + style.tabPadding * 2;
            tabWidths[i] = tabW;

            if (currentRowWidth > 0 && currentRowWidth + tabW + style.tabSpacing > totalW) {
                // Start new row
                currentRow++;
                currentRowWidth = 0;
            }

            tabRows[i] = currentRow;
            currentRowWidth += tabW + style.tabSpacing;
        }

        int rowCount = currentRow + 1;
        float totalHeight = rowCount * tabH + (rowCount - 1) * style.rowSpacing;

        ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + totalHeight));
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, tabId)) return current;

        // Background
        dl->AddRectFilled(pos, ImVec2(pos.x + totalW, pos.y + totalHeight), ColorToU32(colors.BackgroundAlt), sizes.Rounding);

        // Render tabs
        std::vector<float> rowXOffsets(rowCount, 0.0f);

        for (int i = 0; i < count; i++) {
            int row = tabRows[i];
            float tabW = tabWidths[i];
            float tabY = pos.y + row * (tabH + style.rowSpacing);
            float tabX = pos.x + rowXOffsets[row];

            ImVec2 tabPos(tabX, tabY);
            ImRect tabBB(tabPos, ImVec2(tabPos.x + tabW, tabPos.y + tabH));

            bool isActive = (i == current);
            bool hovered = ImGui::IsMouseHoveringRect(tabBB.Min, tabBB.Max);

            // Tab background
            if (isActive) {
                ImVec4 baseColor = ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f);
                ImU32 gradLeft = ColorToU32(ImVec4(
                    baseColor.x * 0.7f, baseColor.y * 0.7f, baseColor.z * 0.7f, baseColor.w
                ));
                ImU32 gradRight = ColorToU32(ImVec4(
                    std::min(baseColor.x * 1.8f, 1.0f),
                    std::min(baseColor.y * 1.8f, 1.0f),
                    std::min(baseColor.z * 1.8f, 1.0f),
                    baseColor.w
                ));
                dl->AddRectFilledMultiColor(tabBB.Min, tabBB.Max, gradLeft, gradRight, gradRight, gradLeft);
            } else if (hovered) {
                dl->AddRectFilled(tabBB.Min, tabBB.Max, IM_COL32(255, 255, 255, 10), sizes.Rounding);
            }

            // Underline indicator
            if (isActive) {
                dl->AddRectFilled(
                    ImVec2(tabBB.Min.x + 2, tabBB.Max.y - 2),
                    ImVec2(tabBB.Max.x - 2, tabBB.Max.y),
                    ColorToU32(colors.Primary)
                );
            }

            // Build label text
            char fullText[128];
            if (icons && icons[i] && labels && labels[i]) {
                snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
            } else if (labels && labels[i]) {
                snprintf(fullText, sizeof(fullText), "%s", labels[i]);
            } else {
                fullText[0] = '\0';
            }

            ImVec2 textSize = ImGui::CalcTextSize(fullText);
            float textX = tabPos.x + (tabW - textSize.x) * 0.5f;
            float textY = tabPos.y + (tabH - textSize.y) * 0.5f;

            ImU32 textColor = isActive ? ColorToU32(colors.Primary) : ColorToU32(colors.TextSecondary);
            dl->AddText(ImVec2(textX, textY), textColor, fullText);

            if (hovered && ImGui::IsMouseClicked(0)) {
                result = i;
            }

            rowXOffsets[row] += tabW + style.tabSpacing;
        }
    } else {
        // Non-wrapping mode (same as original TabBarSmallIcon)
        float tabW = totalW / count;

        ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + tabH));
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, tabId)) return current;

        // Background
        dl->AddRectFilled(pos, ImVec2(pos.x + totalW, pos.y + tabH), ColorToU32(colors.BackgroundAlt), sizes.Rounding);

        for (int i = 0; i < count; i++) {
            ImVec2 tabPos(pos.x + i * tabW, pos.y);
            ImRect tabBB(tabPos, ImVec2(tabPos.x + tabW, tabPos.y + tabH));

            bool isActive = (i == current);
            bool hovered = ImGui::IsMouseHoveringRect(tabBB.Min, tabBB.Max);

            // Tab background
            if (isActive) {
                ImVec4 baseColor = ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f);
                ImU32 gradLeft = ColorToU32(ImVec4(
                    baseColor.x * 0.7f, baseColor.y * 0.7f, baseColor.z * 0.7f, baseColor.w
                ));
                ImU32 gradRight = ColorToU32(ImVec4(
                    std::min(baseColor.x * 1.8f, 1.0f),
                    std::min(baseColor.y * 1.8f, 1.0f),
                    std::min(baseColor.z * 1.8f, 1.0f),
                    baseColor.w
                ));
                dl->AddRectFilledMultiColor(tabBB.Min, tabBB.Max, gradLeft, gradRight, gradRight, gradLeft);
            } else if (hovered) {
                dl->AddRectFilled(tabBB.Min, tabBB.Max, IM_COL32(255, 255, 255, 10), sizes.Rounding);
            }

            // Underline indicator
            if (isActive) {
                dl->AddRectFilled(
                    ImVec2(tabBB.Min.x + 2, tabBB.Max.y - 2),
                    ImVec2(tabBB.Max.x - 2, tabBB.Max.y),
                    ColorToU32(colors.Primary)
                );
            }

            // Build label text
            char fullText[128];
            if (icons && icons[i] && labels && labels[i]) {
                snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
            } else if (labels && labels[i]) {
                snprintf(fullText, sizeof(fullText), "%s", labels[i]);
            } else {
                fullText[0] = '\0';
            }

            ImVec2 textSize = ImGui::CalcTextSize(fullText);
            float textX = tabPos.x + (tabW - textSize.x) * 0.5f;
            float textY = tabPos.y + (tabH - textSize.y) * 0.5f;

            ImU32 textColor = isActive ? ColorToU32(colors.Primary) : ColorToU32(colors.TextSecondary);
            dl->AddText(ImVec2(textX, textY), textColor, fullText);

            if (hovered && ImGui::IsMouseClicked(0)) {
                result = i;
            }
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
// RadioButton
//-----------------------------------------------------------------------------

int RadioButtonGroup(const char* id, const char** labels, int count, int current) {
    return RadioButtonGroupIcon(id, nullptr, labels, count, current);
}

int RadioButtonGroupIcon(const char* id, const char** icons, const char** labels, int count, int current) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return current;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID groupId = window->GetID(id);
    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;
    float height = sizes.ButtonHeight;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + height));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, groupId)) return current;

    ImDrawList* dl = window->DrawList;

    // Background (shared border for all buttons)
    dl->AddRectFilled(pos, ImVec2(pos.x + totalW, pos.y + height), ColorToU32(colors.BackgroundAlt), sizes.Rounding);
    dl->AddRect(pos, ImVec2(pos.x + totalW, pos.y + height), ColorToU32(colors.Border), sizes.Rounding);

    int result = current;
    float buttonW = totalW / count;

    for (int i = 0; i < count; i++) {
        ImVec2 btnPos(pos.x + i * buttonW, pos.y);
        ImRect btnBB(btnPos, ImVec2(btnPos.x + buttonW, btnPos.y + height));

        bool isActive = (i == current);
        bool hovered = ImGui::IsMouseHoveringRect(btnBB.Min, btnBB.Max);

        // Button background
        if (isActive) {
            // Active button has filled primary color
            float rounding = 0.0f;
            ImDrawFlags flags = ImDrawFlags_None;
            if (i == 0) {
                rounding = sizes.Rounding;
                flags = ImDrawFlags_RoundCornersLeft;
            } else if (i == count - 1) {
                rounding = sizes.Rounding;
                flags = ImDrawFlags_RoundCornersRight;
            }
            dl->AddRectFilled(btnBB.Min, btnBB.Max, ColorToU32(colors.Primary), rounding, flags);
        } else if (hovered) {
            dl->AddRectFilled(btnBB.Min, btnBB.Max, IM_COL32(255, 255, 255, 15));
        }

        // Divider line between buttons (except last)
        if (i < count - 1 && !isActive && (i + 1 != current)) {
            dl->AddLine(
                ImVec2(btnBB.Max.x, btnBB.Min.y + 6),
                ImVec2(btnBB.Max.x, btnBB.Max.y - 6),
                ColorToU32(colors.Border)
            );
        }

        // Build label text
        char fullText[128];
        if (icons && icons[i] && labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
        } else if (labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s", labels[i]);
        } else {
            fullText[0] = '\0';
        }

        ImVec2 textSize = ImGui::CalcTextSize(fullText);
        float textX = btnPos.x + (buttonW - textSize.x) * 0.5f;
        float textY = btnPos.y + (height - textSize.y) * 0.5f;

        ImU32 textColor = isActive ? IM_COL32(255, 255, 255, 255) : ColorToU32(colors.TextSecondary);
        dl->AddText(ImVec2(textX, textY), textColor, fullText);

        // Handle click
        if (hovered && ImGui::IsMouseClicked(0)) {
            result = i;
        }
    }

    return result;
}

bool RadioButton(const char* label, int* v, int buttonValue) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float totalW = ImGui::GetContentRegionAvail().x;
    float radioSize = sizes.CheckboxSize;
    float rowH = std::max(radioSize, ImGui::GetTextLineHeight()) + sizes.FramePadding;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;

    bool isSelected = (*v == buttonValue);
    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    if (clicked && !isSelected) {
        *v = buttonValue;
    }

    // Animate selection
    float animT = Animate(id, isSelected ? 1.0f : 0.0f, 12.0f);

    // Label (left)
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float textY = pos.y + (rowH - labelSize.y) * 0.5f;
    dl->AddText(ImVec2(pos.x, textY), ColorToU32(colors.Text), label);

    // Radio circle (right)
    float radioX = pos.x + totalW - radioSize;
    float radioY = pos.y + (rowH - radioSize) * 0.5f;
    ImVec2 center(radioX + radioSize * 0.5f, radioY + radioSize * 0.5f);
    float radius = radioSize * 0.5f;

    // Outer circle
    ImU32 borderColor = hovered ? ColorToU32(colors.Primary) : ColorToU32(colors.Border);
    dl->AddCircle(center, radius, borderColor, 0, 1.5f);

    // Inner filled circle (animated)
    if (animT > 0.01f) {
        float innerRadius = radius * 0.5f * animT;
        dl->AddCircleFilled(center, innerRadius, ColorToU32(colors.Primary));
    }

    return clicked;
}

bool RadioButtonClassic(const char* label, int* v, int buttonValue) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float radioSize = sizes.CheckboxSize;
    float labelPad = 8.0f;
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float totalW = radioSize + labelPad + labelSize.x;
    float rowH = std::max(radioSize, ImGui::GetTextLineHeight()) + sizes.FramePadding;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;

    bool isSelected = (*v == buttonValue);
    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    if (clicked && !isSelected) {
        *v = buttonValue;
    }

    // Animate selection
    float animT = Animate(id, isSelected ? 1.0f : 0.0f, 12.0f);

    // Radio circle (left)
    float radioY = pos.y + (rowH - radioSize) * 0.5f;
    ImVec2 center(pos.x + radioSize * 0.5f, radioY + radioSize * 0.5f);
    float radius = radioSize * 0.5f;

    // Outer circle
    ImU32 borderColor = hovered ? ColorToU32(colors.Primary) : ColorToU32(colors.Border);
    dl->AddCircle(center, radius, borderColor, 0, 1.5f);

    // Inner filled circle (animated)
    if (animT > 0.01f) {
        float innerRadius = radius * 0.5f * animT;
        dl->AddCircleFilled(center, innerRadius, ColorToU32(colors.Primary));
    }

    // Label (right)
    float textX = pos.x + radioSize + labelPad;
    float textY = pos.y + (rowH - labelSize.y) * 0.5f;
    dl->AddText(ImVec2(textX, textY), ColorToU32(colors.Text), label);

    return clicked;
}

//-----------------------------------------------------------------------------
// SubTab
//-----------------------------------------------------------------------------

int SubTab(const char* id, const char** labels, int count, int current, SubTabStyle style) {
    return SubTabIcon(id, nullptr, labels, count, current, style);
}

int SubTabIcon(const char* id, const char** icons, const char** labels, int count, int current, SubTabStyle style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return current;

    const auto& colors = GetColorScheme();

    ImGuiID tabId = window->GetID(id);
    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;
    float height = 28.0f;  // Smaller height than regular tabs

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + height));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, tabId)) return current;

    ImDrawList* dl = window->DrawList;

    // Calculate individual tab widths based on content
    float totalTextW = 0.0f;
    float padding = 16.0f;  // Horizontal padding per tab
    float spacing = 8.0f;   // Space between tabs

    // Pre-calculate all widths
    float tabWidths[16];
    for (int i = 0; i < count && i < 16; i++) {
        char fullText[128];
        if (icons && icons[i] && labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
        } else if (labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s", labels[i]);
        } else {
            fullText[0] = '\0';
        }
        ImVec2 textSize = ImGui::CalcTextSize(fullText);
        tabWidths[i] = textSize.x + padding * 2;
        totalTextW += tabWidths[i];
    }
    totalTextW += spacing * (count - 1);

    // Start position (centered if smaller than available width)
    float startX = pos.x;
    if (totalTextW < totalW) {
        // Left-align for subtabs
        startX = pos.x;
    }

    int result = current;
    float currentX = startX;

    for (int i = 0; i < count; i++) {
        float tabW = tabWidths[i];
        ImVec2 tabPos(currentX, pos.y);
        ImRect tabBB(tabPos, ImVec2(tabPos.x + tabW, tabPos.y + height));

        bool isActive = (i == current);
        bool hovered = ImGui::IsMouseHoveringRect(tabBB.Min, tabBB.Max);

        // Build label text
        char fullText[128];
        if (icons && icons[i] && labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s %s", icons[i], labels[i]);
        } else if (labels && labels[i]) {
            snprintf(fullText, sizeof(fullText), "%s", labels[i]);
        } else {
            fullText[0] = '\0';
        }

        if (style == SubTabStyle::Gradient) {
            // Gradient style - like Tab with gradient background
            if (isActive) {
                ImVec4 baseColor = ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f);
                ImU32 gradLeft = ColorToU32(ImVec4(
                    baseColor.x * 0.7f, baseColor.y * 0.7f, baseColor.z * 0.7f, baseColor.w
                ));
                ImU32 gradRight = ColorToU32(ImVec4(
                    std::min(baseColor.x * 1.8f, 1.0f),
                    std::min(baseColor.y * 1.8f, 1.0f),
                    std::min(baseColor.z * 1.8f, 1.0f),
                    baseColor.w
                ));
                dl->AddRectFilledMultiColor(tabBB.Min, tabBB.Max, gradLeft, gradRight, gradRight, gradLeft);
            } else if (hovered) {
                dl->AddRectFilled(tabBB.Min, tabBB.Max, IM_COL32(255, 255, 255, 15), height * 0.5f);
            }
        } else if (style == SubTabStyle::Pill) {
            // Pill style - rounded background for active/hovered (solid color)
            if (isActive) {
                dl->AddRectFilled(tabBB.Min, tabBB.Max,
                    ColorToU32(ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.25f)),
                    height * 0.5f);
            } else if (hovered) {
                dl->AddRectFilled(tabBB.Min, tabBB.Max,
                    IM_COL32(255, 255, 255, 15),
                    height * 0.5f);
            }
        } else {
            // Underline style - hover background + underline indicator
            if (hovered && !isActive) {
                dl->AddRectFilled(tabBB.Min, tabBB.Max, IM_COL32(255, 255, 255, 10));
            }

            // Underline indicator for active
            if (isActive) {
                float indicatorH = 2.0f;
                dl->AddRectFilled(
                    ImVec2(tabBB.Min.x + padding * 0.5f, tabBB.Max.y - indicatorH),
                    ImVec2(tabBB.Max.x - padding * 0.5f, tabBB.Max.y),
                    ColorToU32(colors.Primary)
                );
            }
        }

        // Text
        ImVec2 textSize = ImGui::CalcTextSize(fullText);
        float textX = tabPos.x + (tabW - textSize.x) * 0.5f;
        float textY = tabPos.y + (height - textSize.y) * 0.5f;

        ImU32 textColor = isActive ? ColorToU32(colors.Primary) : ColorToU32(colors.TextSecondary);
        dl->AddText(ImVec2(textX, textY), textColor, fullText);

        // Handle click
        if (hovered && ImGui::IsMouseClicked(0)) {
            result = i;
        }

        currentX += tabW + spacing;
    }

    return result;
}

//-----------------------------------------------------------------------------
// Hotkey Input
//-----------------------------------------------------------------------------

static ImGuiID g_activeHotkeyId = 0;

bool HotkeyInput(const char* label, HotkeyBinding* binding) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float totalW = ImGui::GetContentRegionAvail().x;
    float buttonW = 140.0f;
    float clearW = 24.0f;
    float rowH = sizes.ButtonHeight;

    ImVec2 labelSize = ImGui::CalcTextSize(label);

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;
    bool changed = false;
    bool isBinding = (g_activeHotkeyId == id);

    // Label (left)
    float textY = pos.y + (rowH - labelSize.y) * 0.5f;
    dl->AddText(ImVec2(pos.x, textY), ColorToU32(colors.Text), label);

    // Button area (right)
    float buttonX = pos.x + totalW - buttonW - clearW - sizes.ItemSpacing;
    ImRect buttonBB(ImVec2(buttonX, pos.y), ImVec2(buttonX + buttonW, pos.y + rowH));

    bool buttonHovered = ImGui::IsMouseHoveringRect(buttonBB.Min, buttonBB.Max);

    // Button background
    ImVec4 bgColor = isBinding ? colors.PrimaryActive :
                    (buttonHovered ? colors.BorderHover : colors.Border);
    dl->AddRectFilled(buttonBB.Min, buttonBB.Max, ColorToU32(bgColor), sizes.Rounding);

    // Button text
    const char* displayText = isBinding ? "Press key..." : GetHotkeyDisplayString(*binding);
    ImVec2 textSize = ImGui::CalcTextSize(displayText);
    float btnTextX = buttonX + (buttonW - textSize.x) * 0.5f;
    float btnTextY = pos.y + (rowH - textSize.y) * 0.5f;
    dl->AddText(ImVec2(btnTextX, btnTextY), ColorToU32(isBinding ? colors.Text : colors.TextSecondary), displayText);

    // Handle button click to start binding
    if (buttonHovered && ImGui::IsMouseClicked(0) && !isBinding) {
        g_activeHotkeyId = id;
    }

    // Handle key input when binding
    if (isBinding) {
#ifdef _WIN32
        // Check for mouse button press first (VK_LBUTTON=0x01, VK_RBUTTON=0x02, VK_MBUTTON=0x04, VK_XBUTTON1=0x05, VK_XBUTTON2=0x06)
        // Note: Skip left mouse button (0x01) to avoid conflict with UI clicking
        int mouseButtons[] = { 0x02, 0x04, 0x05, 0x06 }; // Mouse2-5
        for (int mb : mouseButtons) {
            if (GetAsyncKeyState(mb) & 0x8000) {
                binding->key = mb;
                binding->ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                binding->alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
                binding->shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                g_activeHotkeyId = 0;
                changed = true;
                break;
            }
        }

        // Check for keyboard key press
        if (!changed) {
            for (int vk = 0x08; vk <= 0xFE; vk++) {
                // Skip modifier keys
                if (vk == VK_CONTROL || vk == VK_SHIFT || vk == VK_MENU ||
                    vk == VK_LCONTROL || vk == VK_RCONTROL ||
                    vk == VK_LSHIFT || vk == VK_RSHIFT ||
                    vk == VK_LMENU || vk == VK_RMENU) {
                    continue;
                }

                if (GetAsyncKeyState(vk) & 0x8000) {
                    // Check if this is escape to cancel
                    if (vk == VK_ESCAPE) {
                        g_activeHotkeyId = 0;
                        break;
                    }

                    binding->key = vk;
                    binding->ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                    binding->alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
                    binding->shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                    g_activeHotkeyId = 0;
                    changed = true;
                    break;
                }
            }
        }
#endif
    }

    // Clear button
    float clearX = buttonX + buttonW + sizes.ItemSpacing;
    ImRect clearBB(ImVec2(clearX, pos.y + (rowH - clearW) * 0.5f),
                   ImVec2(clearX + clearW, pos.y + (rowH + clearW) * 0.5f));

    bool clearHovered = ImGui::IsMouseHoveringRect(clearBB.Min, clearBB.Max);

    dl->AddRectFilled(clearBB.Min, clearBB.Max,
                     clearHovered ? ColorToU32(colors.Error) : IM_COL32(80, 80, 90, 255),
                     sizes.Rounding * 0.5f);

    // X icon for clear
    float xPad = 6.0f;
    ImU32 xColor = IM_COL32(255, 255, 255, 200);
    dl->AddLine(ImVec2(clearBB.Min.x + xPad, clearBB.Min.y + xPad),
               ImVec2(clearBB.Max.x - xPad, clearBB.Max.y - xPad), xColor, 1.5f);
    dl->AddLine(ImVec2(clearBB.Max.x - xPad, clearBB.Min.y + xPad),
               ImVec2(clearBB.Min.x + xPad, clearBB.Max.y - xPad), xColor, 1.5f);

    if (clearHovered && ImGui::IsMouseClicked(0)) {
        binding->Clear();
        g_activeHotkeyId = 0;
        changed = true;
    }

    return changed;
}

//-----------------------------------------------------------------------------
// TextInput
//-----------------------------------------------------------------------------

bool TextInput(const char* label, char* buf, size_t bufSize, TextInputLabelMode labelMode, const char* placeholder) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float totalW = ImGui::GetContentRegionAvail().x;
    float inputH = sizes.ButtonHeight;
    float labelH = (labelMode == TextInputLabelMode::Above || labelMode == TextInputLabelMode::Below)
                   ? ImGui::GetTextLineHeight() + 4.0f : 0.0f;
    float totalH = inputH + labelH;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + totalH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;

    // Calculate positions based on label mode
    float labelY = pos.y;
    float inputY = pos.y;

    if (labelMode == TextInputLabelMode::Above) {
        inputY = pos.y + labelH;
    } else if (labelMode == TextInputLabelMode::Below) {
        labelY = pos.y + inputH + 4.0f;
    }

    ImRect inputBB(ImVec2(pos.x, inputY), ImVec2(pos.x + totalW, inputY + inputH));

    // Focus handling
    bool hovered = ImGui::IsMouseHoveringRect(inputBB.Min, inputBB.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    static ImGuiID activeInputId = 0;
    bool isFocused = (activeInputId == id);

    if (clicked) {
        activeInputId = id;
        isFocused = true;
    }

    // Click outside to unfocus
    if (isFocused && ImGui::IsMouseClicked(0) && !hovered) {
        activeInputId = 0;
        isFocused = false;
    }

    // Draw label (Above or Below mode)
    if (labelMode == TextInputLabelMode::Above || labelMode == TextInputLabelMode::Below) {
        dl->AddText(ImVec2(pos.x, labelY), ColorToU32(colors.Text), label);
    }

    // Input background
    ImVec4 bgColor = isFocused ? colors.BackgroundAlt : colors.Background;
    dl->AddRectFilled(inputBB.Min, inputBB.Max, ColorToU32(bgColor), sizes.Rounding);

    // Border (highlight when focused)
    ImU32 borderColor = isFocused ? ColorToU32(colors.Primary) :
                        (hovered ? ColorToU32(colors.BorderHover) : ColorToU32(colors.Border));
    dl->AddRect(inputBB.Min, inputBB.Max, borderColor, sizes.Rounding, 0, isFocused ? 2.0f : 1.0f);

    // Text content area
    float textPadding = sizes.FramePadding;
    ImVec2 textPos(inputBB.Min.x + textPadding, inputBB.Min.y + (inputH - ImGui::GetTextLineHeight()) * 0.5f);

    // Show placeholder or actual text
    bool isEmpty = (buf[0] == '\0');

    if (isEmpty && !isFocused) {
        // Show placeholder
        const char* placeholderText = (labelMode == TextInputLabelMode::Placeholder) ? label :
                                      (placeholder ? placeholder : "");
        if (placeholderText[0] != '\0') {
            dl->AddText(textPos, ColorToU32(colors.TextDisabled), placeholderText);
        }
    } else {
        // Show actual text
        dl->AddText(textPos, ColorToU32(colors.Text), buf);
    }

    // Handle text input when focused
    bool changed = false;
    if (isFocused) {
        // Draw cursor
        float cursorX = textPos.x + ImGui::CalcTextSize(buf).x;
        float cursorY1 = inputBB.Min.y + 6.0f;
        float cursorY2 = inputBB.Max.y - 6.0f;

        // Blinking cursor
        if (fmodf((float)ImGui::GetTime(), 1.0f) < 0.5f) {
            dl->AddLine(ImVec2(cursorX + 1, cursorY1), ImVec2(cursorX + 1, cursorY2),
                       ColorToU32(colors.Text), 1.5f);
        }

        // Handle keyboard input
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
            unsigned int c = io.InputQueueCharacters[i];
            if (c >= 32 && c < 127) { // Printable ASCII
                size_t len = strlen(buf);
                if (len + 1 < bufSize) {
                    buf[len] = (char)c;
                    buf[len + 1] = '\0';
                    changed = true;
                }
            }
        }

        // Handle backspace
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
            size_t len = strlen(buf);
            if (len > 0) {
                buf[len - 1] = '\0';
                changed = true;
            }
        }

        // Handle escape to unfocus
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            activeInputId = 0;
        }

        // Handle enter to unfocus
        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
            activeInputId = 0;
        }
    }

    return changed;
}

bool PasswordInput(const char* label, char* buf, size_t bufSize, TextInputLabelMode labelMode, const char* placeholder) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float totalW = ImGui::GetContentRegionAvail().x;
    float inputH = sizes.ButtonHeight;
    float labelH = (labelMode == TextInputLabelMode::Above || labelMode == TextInputLabelMode::Below)
                   ? ImGui::GetTextLineHeight() + 4.0f : 0.0f;
    float totalH = inputH + labelH;

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + totalH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;

    // Calculate positions based on label mode
    float labelY = pos.y;
    float inputY = pos.y;

    if (labelMode == TextInputLabelMode::Above) {
        inputY = pos.y + labelH;
    } else if (labelMode == TextInputLabelMode::Below) {
        labelY = pos.y + inputH + 4.0f;
    }

    ImRect inputBB(ImVec2(pos.x, inputY), ImVec2(pos.x + totalW, inputY + inputH));

    // Focus handling
    bool hovered = ImGui::IsMouseHoveringRect(inputBB.Min, inputBB.Max);
    bool clicked = hovered && ImGui::IsMouseClicked(0);

    static ImGuiID activePasswordId = 0;
    bool isFocused = (activePasswordId == id);

    if (clicked) {
        activePasswordId = id;
        isFocused = true;
    }

    // Click outside to unfocus
    if (isFocused && ImGui::IsMouseClicked(0) && !hovered) {
        activePasswordId = 0;
        isFocused = false;
    }

    // Draw label (Above or Below mode)
    if (labelMode == TextInputLabelMode::Above || labelMode == TextInputLabelMode::Below) {
        dl->AddText(ImVec2(pos.x, labelY), ColorToU32(colors.Text), label);
    }

    // Input background
    ImVec4 bgColor = isFocused ? colors.BackgroundAlt : colors.Background;
    dl->AddRectFilled(inputBB.Min, inputBB.Max, ColorToU32(bgColor), sizes.Rounding);

    // Border (highlight when focused)
    ImU32 borderColor = isFocused ? ColorToU32(colors.Primary) :
                        (hovered ? ColorToU32(colors.BorderHover) : ColorToU32(colors.Border));
    dl->AddRect(inputBB.Min, inputBB.Max, borderColor, sizes.Rounding, 0, isFocused ? 2.0f : 1.0f);

    // Text content area
    float textPadding = sizes.FramePadding;
    ImVec2 textPos(inputBB.Min.x + textPadding, inputBB.Min.y + (inputH - ImGui::GetTextLineHeight()) * 0.5f);

    // Show placeholder or masked text
    bool isEmpty = (buf[0] == '\0');

    if (isEmpty && !isFocused) {
        // Show placeholder
        const char* placeholderText = (labelMode == TextInputLabelMode::Placeholder) ? label :
                                      (placeholder ? placeholder : "");
        if (placeholderText[0] != '\0') {
            dl->AddText(textPos, ColorToU32(colors.TextDisabled), placeholderText);
        }
    } else {
        // Show masked text (dots)
        size_t len = strlen(buf);
        char masked[256];
        size_t maskLen = std::min(len, sizeof(masked) - 1);
        for (size_t i = 0; i < maskLen; i++) {
            masked[i] = '*';
        }
        masked[maskLen] = '\0';
        dl->AddText(textPos, ColorToU32(colors.Text), masked);
    }

    // Handle text input when focused
    bool changed = false;
    if (isFocused) {
        // Draw cursor
        size_t len = strlen(buf);
        char masked[256];
        size_t maskLen = std::min(len, sizeof(masked) - 1);
        for (size_t i = 0; i < maskLen; i++) {
            masked[i] = '*';
        }
        masked[maskLen] = '\0';

        float cursorX = textPos.x + ImGui::CalcTextSize(masked).x;
        float cursorY1 = inputBB.Min.y + 6.0f;
        float cursorY2 = inputBB.Max.y - 6.0f;

        // Blinking cursor
        if (fmodf((float)ImGui::GetTime(), 1.0f) < 0.5f) {
            dl->AddLine(ImVec2(cursorX + 1, cursorY1), ImVec2(cursorX + 1, cursorY2),
                       ColorToU32(colors.Text), 1.5f);
        }

        // Handle keyboard input
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
            unsigned int c = io.InputQueueCharacters[i];
            if (c >= 32 && c < 127) { // Printable ASCII
                size_t len = strlen(buf);
                if (len + 1 < bufSize) {
                    buf[len] = (char)c;
                    buf[len + 1] = '\0';
                    changed = true;
                }
            }
        }

        // Handle backspace
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
            size_t len = strlen(buf);
            if (len > 0) {
                buf[len - 1] = '\0';
                changed = true;
            }
        }

        // Handle escape to unfocus
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            activePasswordId = 0;
        }

        // Handle enter to unfocus
        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
            activePasswordId = 0;
        }
    }

    return changed;
}

//-----------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------

bool Button(const char* label, const ImVec2& size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    ImVec2 btnSize = size;
    if (btnSize.x <= 0) btnSize.x = ImGui::CalcTextSize(label).x + sizes.FramePadding * 2;
    if (btnSize.y <= 0) btnSize.y = sizes.ButtonHeight;

    ImRect bb(pos, ImVec2(pos.x + btnSize.x, pos.y + btnSize.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList* dl = window->DrawList;

    // Background
    ImVec4 bgColor = held ? colors.PrimaryActive : (hovered ? colors.BorderHover : colors.Border);
    dl->AddRectFilled(bb.Min, bb.Max, ColorToU32(bgColor), sizes.Rounding);

    // Text
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float textX = pos.x + (btnSize.x - textSize.x) * 0.5f;
    float textY = pos.y + (btnSize.y - textSize.y) * 0.5f;
    dl->AddText(ImVec2(textX, textY), ColorToU32(colors.Text), label);

    return pressed;
}

bool ButtonGradient(const char* label, const ImVec2& size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    ImVec2 btnSize = size;
    if (btnSize.x <= 0) btnSize.x = ImGui::CalcTextSize(label).x + sizes.FramePadding * 2;
    if (btnSize.y <= 0) btnSize.y = sizes.ButtonHeight;

    ImRect bb(pos, ImVec2(pos.x + btnSize.x, pos.y + btnSize.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList* dl = window->DrawList;

    // Horizontal gradient background (matching slider style: 0.7x to 1.8x brightness)
    ImVec4 baseColor = held ? colors.PrimaryActive : (hovered ? colors.PrimaryHover : colors.Primary);

    ImU32 gradLeft = ColorToU32(ImVec4(
        baseColor.x * 0.7f,
        baseColor.y * 0.7f,
        baseColor.z * 0.7f,
        baseColor.w
    ));
    ImU32 gradRight = ColorToU32(ImVec4(
        std::min(baseColor.x * 1.8f, 1.0f),
        std::min(baseColor.y * 1.8f, 1.0f),
        std::min(baseColor.z * 1.8f, 1.0f),
        baseColor.w
    ));

    // Draw gradient with rounding
    dl->AddRectFilled(bb.Min, bb.Max, gradLeft, sizes.Rounding);
    dl->AddRectFilledMultiColor(
        bb.Min, bb.Max,
        gradLeft, gradRight, gradRight, gradLeft
    );

    // Re-draw with rounding to clip corners
    dl->AddRect(bb.Min, bb.Max, IM_COL32(0, 0, 0, 0), sizes.Rounding);

    // Text
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float textX = pos.x + (btnSize.x - textSize.x) * 0.5f;
    float textY = pos.y + (btnSize.y - textSize.y) * 0.5f;
    dl->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), label);

    return pressed;
}

bool ButtonOutline(const char* label, const ImVec2& size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    ImVec2 btnSize = size;
    if (btnSize.x <= 0) btnSize.x = ImGui::CalcTextSize(label).x + sizes.FramePadding * 2;
    if (btnSize.y <= 0) btnSize.y = sizes.ButtonHeight;

    ImRect bb(pos, ImVec2(pos.x + btnSize.x, pos.y + btnSize.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList* dl = window->DrawList;

    // Background (subtle on hover)
    if (hovered || held) {
        dl->AddRectFilled(bb.Min, bb.Max, ColorToU32(ImVec4(colors.Primary.x, colors.Primary.y, colors.Primary.z, 0.1f)), sizes.Rounding);
    }

    // Border
    ImVec4 borderColor = held ? colors.PrimaryActive : (hovered ? colors.PrimaryHover : colors.Primary);
    dl->AddRect(bb.Min, bb.Max, ColorToU32(borderColor), sizes.Rounding, 0, 1.5f);

    // Text
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float textX = pos.x + (btnSize.x - textSize.x) * 0.5f;
    float textY = pos.y + (btnSize.y - textSize.y) * 0.5f;
    dl->AddText(ImVec2(textX, textY), ColorToU32(borderColor), label);

    return pressed;
}

bool ButtonIcon(const char* icon, const char* label, const ImVec2& size) {
    char fullLabel[256];
    if (icon && icon[0]) {
        snprintf(fullLabel, sizeof(fullLabel), "%s  %s", icon, label);
    } else {
        snprintf(fullLabel, sizeof(fullLabel), "%s", label);
    }
    return Button(fullLabel, size);
}

//-----------------------------------------------------------------------------
// ComboBox
//-----------------------------------------------------------------------------

bool Combo(const char* label, int* current, const char** items, int count) {
    return ComboEx(label, current, items, count, 0);
}

bool ComboEx(const char* label, int* current, const char** items, int count, float width) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float totalW = ImGui::GetContentRegionAvail().x;
    float comboW = width > 0 ? width : 150.0f;
    float rowH = sizes.ButtonHeight;

    ImVec2 labelSize = ImGui::CalcTextSize(label);

    ImRect bb(pos, ImVec2(pos.x + totalW, pos.y + rowH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;
    bool changed = false;

    // Label (left)
    float textY = pos.y + (rowH - labelSize.y) * 0.5f;
    dl->AddText(ImVec2(pos.x, textY), ColorToU32(colors.Text), label);

    // Combo button (right)
    float comboX = pos.x + totalW - comboW;
    ImRect comboBB(ImVec2(comboX, pos.y), ImVec2(comboX + comboW, pos.y + rowH));

    bool hovered = ImGui::IsMouseHoveringRect(comboBB.Min, comboBB.Max);

    // Background
    dl->AddRectFilled(comboBB.Min, comboBB.Max, ColorToU32(hovered ? colors.BorderHover : colors.Border), sizes.Rounding);

    // Current item text
    const char* preview = (*current >= 0 && *current < count) ? items[*current] : "";
    ImVec2 previewSize = ImGui::CalcTextSize(preview);
    float previewX = comboX + sizes.FramePadding;
    float previewY = pos.y + (rowH - previewSize.y) * 0.5f;
    dl->AddText(ImVec2(previewX, previewY), ColorToU32(colors.Text), preview);

    // Arrow indicator
    float arrowX = comboX + comboW - 20;
    float arrowY = pos.y + rowH * 0.5f;
    dl->AddTriangleFilled(
        ImVec2(arrowX, arrowY - 3),
        ImVec2(arrowX + 8, arrowY - 3),
        ImVec2(arrowX + 4, arrowY + 3),
        ColorToU32(colors.TextSecondary)
    );

    // Handle click to open popup
    ImGui::PushID(id);
    if (hovered && ImGui::IsMouseClicked(0)) {
        ImGui::OpenPopup("##ComboPopup");
    }

    // Popup
    ImGui::SetNextWindowPos(ImVec2(comboX, pos.y + rowH));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, colors.BackgroundAlt);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, sizes.Rounding);

    if (ImGui::BeginPopup("##ComboPopup")) {
        for (int i = 0; i < count; i++) {
            bool isSelected = (i == *current);
            if (ImGui::Selectable(items[i], isSelected)) {
                *current = i;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    ImGui::PopID();

    return changed;
}

bool LanguageCombo(const char* label, std::string& currentLang, float width) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    // Get available languages from i18n system
    const auto& languages = i18n::Localization::Instance().GetAvailableLanguages();
    if (languages.empty()) return false;

    ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;

    float comboW = width > 0 ? width : 100.0f;
    float rowH = sizes.ButtonHeight;

    ImRect bb(pos, ImVec2(pos.x + comboW, pos.y + rowH));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = window->DrawList;
    bool changed = false;

    // Find current language display name
    const char* currentDisplay = currentLang.c_str();
    int currentIndex = -1;
    for (size_t i = 0; i < languages.size(); i++) {
        if (languages[i].code == currentLang) {
            currentDisplay = languages[i].nativeName.c_str();
            currentIndex = (int)i;
            break;
        }
    }

    bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);

    // Background
    dl->AddRectFilled(bb.Min, bb.Max, ColorToU32(hovered ? colors.BorderHover : colors.Border), sizes.Rounding);

    // Current item text
    ImVec2 previewSize = ImGui::CalcTextSize(currentDisplay);
    float previewX = pos.x + sizes.FramePadding;
    float previewY = pos.y + (rowH - previewSize.y) * 0.5f;
    dl->AddText(ImVec2(previewX, previewY), ColorToU32(colors.Text), currentDisplay);

    // Arrow indicator
    float arrowX = pos.x + comboW - 20;
    float arrowY = pos.y + rowH * 0.5f;
    dl->AddTriangleFilled(
        ImVec2(arrowX, arrowY - 3),
        ImVec2(arrowX + 8, arrowY - 3),
        ImVec2(arrowX + 4, arrowY + 3),
        ColorToU32(colors.TextSecondary)
    );

    // Handle click to open popup
    ImGui::PushID(id);
    if (hovered && ImGui::IsMouseClicked(0)) {
        ImGui::OpenPopup("##LangComboPopup");
    }

    // Popup
    ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + rowH));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, colors.BackgroundAlt);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, sizes.Rounding);

    if (ImGui::BeginPopup("##LangComboPopup")) {
        for (size_t i = 0; i < languages.size(); i++) {
            bool isSelected = ((int)i == currentIndex);
            if (ImGui::Selectable(languages[i].nativeName.c_str(), isSelected)) {
                currentLang = languages[i].code;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    ImGui::PopID();

    return changed;
}

//-----------------------------------------------------------------------------
// Color Picker
//-----------------------------------------------------------------------------

bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags) {
    // Wrapper around ImGui's color picker with our styling
    ImGui::PushID(label);

    const auto& colors = GetColorScheme();

    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors.Border);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colors.BorderHover);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, colors.BackgroundAlt);

    bool changed = ImGui::ColorEdit4(label, col, flags | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    return changed;
}

//-----------------------------------------------------------------------------
// Separator with Text
//-----------------------------------------------------------------------------

void SeparatorText(const char* text) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    const auto& colors = GetColorScheme();
    const auto& sizes = GetSizeConfig();

    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float lineY = pos.y + textSize.y * 0.5f;

    ImDrawList* dl = window->DrawList;

    // Left line
    float textX = pos.x + 10;
    dl->AddLine(ImVec2(pos.x, lineY), ImVec2(textX - 5, lineY), ColorToU32(colors.Border));

    // Text
    dl->AddText(ImVec2(textX, pos.y), ColorToU32(colors.TextSecondary), text);

    // Right line
    float rightX = textX + textSize.x + 5;
    dl->AddLine(ImVec2(rightX, lineY), ImVec2(pos.x + totalW, lineY), ColorToU32(colors.Border));

    ImGui::ItemSize(ImVec2(totalW, textSize.y + sizes.ItemSpacing));
}

//-----------------------------------------------------------------------------
// Label with Value
//-----------------------------------------------------------------------------

void LabelValue(const char* label, const char* value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    const auto& colors = GetColorScheme();

    ImVec2 pos = window->DC.CursorPos;
    float totalW = ImGui::GetContentRegionAvail().x;

    ImVec2 labelSize = ImGui::CalcTextSize(label);
    ImVec2 valueSize = ImGui::CalcTextSize(value);
    float rowH = std::max(labelSize.y, valueSize.y);

    ImDrawList* dl = window->DrawList;

    // Label (left)
    dl->AddText(pos, ColorToU32(colors.Text), label);

    // Value (right)
    dl->AddText(ImVec2(pos.x + totalW - valueSize.x, pos.y), ColorToU32(colors.TextSecondary), value);

    ImGui::ItemSize(ImVec2(totalW, rowH));
}

} // namespace StyleUI
