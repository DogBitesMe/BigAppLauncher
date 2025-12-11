#include "Theme.h"

namespace Theme {

void Apply() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Rounding
    style.WindowRounding = Size::Rounding;
    style.ChildRounding = Size::Rounding;
    style.FrameRounding = Size::Rounding * 0.5f;
    style.PopupRounding = Size::Rounding;
    style.ScrollbarRounding = Size::Rounding;
    style.GrabRounding = Size::Rounding;
    style.TabRounding = Size::Rounding * 0.5f;

    // Borders
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = Size::BorderWidth;
    style.PopupBorderSize = Size::BorderWidth;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    // Padding and spacing
    style.WindowPadding = ImVec2(Size::FramePadding, Size::FramePadding);
    style.FramePadding = ImVec2(Size::FramePadding, 8.0f);
    style.ItemSpacing = ImVec2(Size::ItemSpacing, Size::ItemSpacing);
    style.ItemInnerSpacing = ImVec2(Size::ItemSpacing, Size::ItemSpacing);

    // Alpha
    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.5f;

    // Colors
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = Colors::Text;
    colors[ImGuiCol_TextDisabled] = Colors::TextDisabled;

    colors[ImGuiCol_WindowBg] = Colors::Panel;
    colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
    colors[ImGuiCol_PopupBg] = Colors::Panel;

    colors[ImGuiCol_Border] = Colors::Border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_FrameBg] = Colors::Input;
    colors[ImGuiCol_FrameBgHovered] = Colors::InputHover;
    colors[ImGuiCol_FrameBgActive] = Colors::InputFocus;

    colors[ImGuiCol_TitleBg] = Colors::Panel;
    colors[ImGuiCol_TitleBgActive] = Colors::Panel;
    colors[ImGuiCol_TitleBgCollapsed] = Colors::Panel;

    colors[ImGuiCol_MenuBarBg] = Colors::Panel;

    colors[ImGuiCol_ScrollbarBg] = Colors::Input;
    colors[ImGuiCol_ScrollbarGrab] = Colors::Tab;
    colors[ImGuiCol_ScrollbarGrabHovered] = Colors::TabHover;
    colors[ImGuiCol_ScrollbarGrabActive] = Colors::Primary;

    colors[ImGuiCol_CheckMark] = Colors::Primary;

    colors[ImGuiCol_SliderGrab] = Colors::Primary;
    colors[ImGuiCol_SliderGrabActive] = Colors::PrimaryActive;

    colors[ImGuiCol_Button] = Colors::Primary;
    colors[ImGuiCol_ButtonHovered] = Colors::PrimaryHover;
    colors[ImGuiCol_ButtonActive] = Colors::PrimaryActive;

    colors[ImGuiCol_Header] = Colors::Tab;
    colors[ImGuiCol_HeaderHovered] = Colors::TabHover;
    colors[ImGuiCol_HeaderActive] = Colors::TabActive;

    colors[ImGuiCol_Separator] = Colors::Border;
    colors[ImGuiCol_SeparatorHovered] = Colors::BorderHover;
    colors[ImGuiCol_SeparatorActive] = Colors::Primary;

    colors[ImGuiCol_ResizeGrip] = Colors::Border;
    colors[ImGuiCol_ResizeGripHovered] = Colors::BorderHover;
    colors[ImGuiCol_ResizeGripActive] = Colors::Primary;

    colors[ImGuiCol_Tab] = Colors::Tab;
    colors[ImGuiCol_TabHovered] = Colors::TabHover;
    colors[ImGuiCol_TabActive] = Colors::TabActive;
    colors[ImGuiCol_TabUnfocused] = Colors::Tab;
    colors[ImGuiCol_TabUnfocusedActive] = Colors::TabActive;

    colors[ImGuiCol_PlotLines] = Colors::Primary;
    colors[ImGuiCol_PlotLinesHovered] = Colors::PrimaryHover;
    colors[ImGuiCol_PlotHistogram] = Colors::Primary;
    colors[ImGuiCol_PlotHistogramHovered] = Colors::PrimaryHover;

    colors[ImGuiCol_TableHeaderBg] = Colors::Panel;
    colors[ImGuiCol_TableBorderStrong] = Colors::Border;
    colors[ImGuiCol_TableBorderLight] = Colors::Border;
    colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.02f);

    colors[ImGuiCol_TextSelectedBg] = ImVec4(Colors::Primary.x, Colors::Primary.y, Colors::Primary.z, 0.35f);

    colors[ImGuiCol_DragDropTarget] = Colors::Primary;

    colors[ImGuiCol_NavHighlight] = Colors::Primary;
    colors[ImGuiCol_NavWindowingHighlight] = Colors::Primary;
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.5f);

    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.5f);
}

ImU32 GetGradientTop(const ImVec4& base) {
    return IM_COL32(
        (int)(base.x * 255 * 1.1f),
        (int)(base.y * 255 * 1.1f),
        (int)(base.z * 255 * 1.1f),
        (int)(base.w * 255)
    );
}

ImU32 GetGradientBottom(const ImVec4& base) {
    return IM_COL32(
        (int)(base.x * 255 * 0.9f),
        (int)(base.y * 255 * 0.9f),
        (int)(base.z * 255 * 0.9f),
        (int)(base.w * 255)
    );
}

} // namespace Theme
