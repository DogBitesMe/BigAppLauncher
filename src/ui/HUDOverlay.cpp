#include "HUDOverlay.h"
#include "IconsFontAwesome6.h"
#include <imgui.h>
#include <algorithm>

HUDOverlay::HUDOverlay() {
    // Add some sample messages
    AddMessage("HUD Overlay initialized", 5.0f);
}

void HUDOverlay::Update(float deltaTime) {
    // Update message timers
    for (auto it = m_messages.begin(); it != m_messages.end();) {
        it->remainingTime -= deltaTime;
        if (it->remainingTime <= 0.0f) {
            it = m_messages.erase(it);
        } else {
            ++it;
        }
    }
}

void HUDOverlay::AddMessage(const std::string& message, float duration) {
    m_messages.push_back({ message, duration });

    // Limit message count
    while (m_messages.size() > 5) {
        m_messages.erase(m_messages.begin());
    }
}

void HUDOverlay::Render(int windowWidth, int windowHeight) {
    if (!m_visible) return;

    RenderHeaderBar(windowWidth);
    RenderFooterBar(windowWidth, windowHeight);
    RenderSideMessages(windowWidth, windowHeight);
    RenderFloatingWindow(windowWidth, windowHeight);
}

void HUDOverlay::RenderHeaderBar(int windowWidth) {
    // Small opaque header bar for FPS and status
    // Designed for overlay - minimal size, dark background

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, HEADER_HEIGHT));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("##HUDHeader", nullptr, flags)) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();

        // FPS display
        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "%s FPS: %.0f", ICON_FA_GAUGE_HIGH, m_fps);

        ImU32 fpsColor = m_fps >= 60 ? IM_COL32(100, 255, 100, 255) :
                        (m_fps >= 30 ? IM_COL32(255, 255, 100, 255) : IM_COL32(255, 100, 100, 255));

        drawList->AddText(
            ImVec2(windowPos.x + 10, windowPos.y + 5),
            fpsColor,
            fpsText
        );

        // Status indicators (right side)
        const char* status = ICON_FA_CIRCLE " Active";
        ImVec2 statusSize = ImGui::CalcTextSize(status);
        drawList->AddText(
            ImVec2(windowPos.x + windowWidth - statusSize.x - 15, windowPos.y + 5),
            IM_COL32(100, 255, 100, 255),
            status
        );

        // Time
        char timeText[32];
        snprintf(timeText, sizeof(timeText), "%s 12:34:56", ICON_FA_CLOCK);
        ImVec2 timeSize = ImGui::CalcTextSize(timeText);
        drawList->AddText(
            ImVec2(windowPos.x + windowWidth * 0.5f - timeSize.x * 0.5f, windowPos.y + 5),
            IM_COL32(200, 200, 200, 255),
            timeText
        );
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void HUDOverlay::RenderFooterBar(int windowWidth, int windowHeight) {
    // Small opaque footer bar for messages

    float y = windowHeight - FOOTER_HEIGHT;

    ImGui::SetNextWindowPos(ImVec2(0, y));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, FOOTER_HEIGHT));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("##HUDFooter", nullptr, flags)) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();

        // Status message
        const char* footerText = ICON_FA_CIRCLE_INFO " Press INSERT to toggle menu | F12 for debug";
        drawList->AddText(
            ImVec2(windowPos.x + 10, windowPos.y + 4),
            IM_COL32(180, 180, 180, 255),
            footerText
        );
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void HUDOverlay::RenderSideMessages(int windowWidth, int windowHeight) {
    if (m_messages.empty()) return;

    // Messages on left side
    float x = 10;
    float y = HEADER_HEIGHT + 20;
    float msgHeight = 22.0f;

    ImGui::SetNextWindowPos(ImVec2(x, y));
    ImGui::SetNextWindowSize(ImVec2(SIDE_WIDTH, m_messages.size() * msgHeight + 10));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);

    if (ImGui::Begin("##HUDMessages", nullptr, flags)) {
        for (const auto& msg : m_messages) {
            // Fade out effect
            float alpha = std::min(msg.remainingTime, 1.0f);
            ImU32 color = IM_COL32(220, 220, 220, (int)(alpha * 255));

            ImGui::TextColored(ImColor(color).Value, "%s", msg.text.c_str());
        }
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();
}

void HUDOverlay::RenderFloatingWindow(int windowWidth, int windowHeight) {
    if (!m_floatingWindowVisible) return;

    // Small draggable info panel
    float width = 180.0f;
    float height = 120.0f;

    ImGui::SetNextWindowPos(m_floatingWindowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));

    if (ImGui::Begin("##FloatingInfo", nullptr, flags)) {
        // Update position when dragged
        m_floatingWindowPos = ImGui::GetWindowPos();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();

        // Header
        drawList->AddText(
            ImVec2(windowPos.x + 10, windowPos.y + 8),
            IM_COL32(66, 150, 255, 255),
            ICON_FA_CHART_LINE " Statistics"
        );

        ImGui::SetCursorPosY(30);
        ImGui::Separator();

        ImGui::Spacing();

        // Stats
        ImGui::Text("Players: 12");
        ImGui::Text("Items: 48");
        ImGui::Text("Distance: 156m");
        ImGui::Text("Health: 100%%");
    }
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}
