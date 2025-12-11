#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include "Widgets.h"
#include "Theme.h"
#include "../i18n/Localization.h"
#include <cmath>

namespace Widgets {

bool GradientButton(const char* label, const ImVec2& size) {
    ImVec2 buttonSize = size;
    if (buttonSize.x == 0) {
        buttonSize.x = ImGui::CalcTextSize(label).x + Theme::Size::FramePadding * 2;
    }
    if (buttonSize.y == 0) {
        buttonSize.y = Theme::Size::ButtonHeight;
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Invisible button for interaction
    ImGui::InvisibleButton(label, buttonSize);
    bool clicked = ImGui::IsItemClicked();
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    // Choose color based on state
    ImVec4 baseColor = Theme::Colors::Primary;
    if (active) {
        baseColor = Theme::Colors::PrimaryActive;
    } else if (hovered) {
        baseColor = Theme::Colors::PrimaryHover;
    }

    // Draw gradient rectangle
    ImU32 colTop = Theme::GetGradientTop(baseColor);
    ImU32 colBottom = Theme::GetGradientBottom(baseColor);

    drawList->AddRectFilledMultiColor(
        pos,
        ImVec2(pos.x + buttonSize.x, pos.y + buttonSize.y),
        colTop, colTop, colBottom, colBottom
    );

    // Draw rounded corners overlay
    drawList->AddRect(
        pos,
        ImVec2(pos.x + buttonSize.x, pos.y + buttonSize.y),
        IM_COL32(255, 255, 255, hovered ? 50 : 25),
        Theme::Size::Rounding * 0.5f,
        0,
        1.0f
    );

    // Draw text centered
    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 textPos = ImVec2(
        pos.x + (buttonSize.x - textSize.x) * 0.5f,
        pos.y + (buttonSize.y - textSize.y) * 0.5f
    );
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), label);

    return clicked;
}

bool InputText(const char* label, char* buf, size_t bufSize, const char* placeholder,
               ImGuiInputTextFlags flags) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Theme::Size::FramePadding, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Theme::Size::Rounding * 0.5f);

    bool result = false;

    // Show placeholder if buffer is empty
    if (placeholder && buf[0] == '\0') {
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors::TextHint);
    }

    result = ImGui::InputText(label, buf, bufSize, flags);

    if (placeholder && buf[0] == '\0') {
        ImGui::PopStyleColor();
        // Draw placeholder text
        ImVec2 pos = ImGui::GetItemRectMin();
        pos.x += Theme::Size::FramePadding;
        pos.y += 10.0f;
        ImGui::GetWindowDrawList()->AddText(pos, ImGui::ColorConvertFloat4ToU32(Theme::Colors::TextHint), placeholder);
    }

    ImGui::PopStyleVar(2);
    return result;
}

bool InputPassword(const char* label, char* buf, size_t bufSize, const char* placeholder) {
    return InputText(label, buf, bufSize, placeholder, ImGuiInputTextFlags_Password);
}

int TabBar(const char* id, const char** labels, int count, int currentIndex) {
    int selectedIndex = currentIndex;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    float totalWidth = ImGui::GetContentRegionAvail().x;
    float tabWidth = totalWidth / count;

    for (int i = 0; i < count; i++) {
        ImGui::PushID(i);

        bool isSelected = (i == currentIndex);
        ImVec4 bgColor = isSelected ? Theme::Colors::TabActive : Theme::Colors::Tab;

        ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isSelected ? bgColor : Theme::Colors::TabHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::Colors::TabActive);

        if (ImGui::Button(labels[i], ImVec2(tabWidth, Theme::Size::TabHeight))) {
            selectedIndex = i;
        }

        ImGui::PopStyleColor(3);

        if (i < count - 1) {
            ImGui::SameLine();
        }

        ImGui::PopID();
    }

    ImGui::PopStyleVar();

    return selectedIndex;
}

void TextCentered(const char* text) {
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text);
}

void Spinner(const char* label, float radius, float thickness) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size(radius * 2, radius * 2 + style.FramePadding.y * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return;

    // Render
    window->DrawList->PathClear();

    int numSegments = 30;
    float start = (float)ImGui::GetTime() * 3.0f;

    const float aMin = start;
    const float aMax = start + IM_PI * 1.5f;

    const ImVec2 center = ImVec2(pos.x + radius, pos.y + style.FramePadding.y + radius);

    for (int i = 0; i < numSegments; i++) {
        const float a = aMin + ((float)i / (float)numSegments) * (aMax - aMin);
        window->DrawList->PathLineTo(ImVec2(
            center.x + cosf(a) * radius,
            center.y + sinf(a) * radius
        ));
    }

    window->DrawList->PathStroke(ImGui::ColorConvertFloat4ToU32(Theme::Colors::Primary), false, thickness);
}

void DrawPanelBackground(const ImVec2& pos, const ImVec2& size, float alpha) {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    ImVec4 bgColor = Theme::Colors::Panel;
    bgColor.w = alpha;

    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImGui::ColorConvertFloat4ToU32(bgColor),
        Theme::Size::Rounding
    );

    // Subtle border
    drawList->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(255, 255, 255, 15),
        Theme::Size::Rounding,
        0,
        1.0f
    );
}

void MessageBox(const char* message, const ImVec4& color) {
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextWrapped("%s", message);
    ImGui::PopStyleColor();
}

bool LanguageSelector(const char* label, std::string& currentLang) {
    bool changed = false;

    const auto& languages = i18n::Localization::Instance().GetAvailableLanguages();

    // Find current language display name
    const char* currentDisplay = currentLang.c_str();
    for (const auto& lang : languages) {
        if (lang.code == currentLang) {
            currentDisplay = lang.nativeName.c_str();
            break;
        }
    }

    if (ImGui::BeginCombo(label, currentDisplay)) {
        for (const auto& lang : languages) {
            bool isSelected = (lang.code == currentLang);
            if (ImGui::Selectable(lang.nativeName.c_str(), isSelected)) {
                currentLang = lang.code;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return changed;
}

} // namespace Widgets
