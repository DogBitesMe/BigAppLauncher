#pragma once

#include <string>
#include <vector>
#include <imgui.h>

class HUDOverlay {
public:
    HUDOverlay();
    ~HUDOverlay() = default;

    // Render all HUD elements
    void Render(int windowWidth, int windowHeight);

    // Update (for animations/timers)
    void Update(float deltaTime);

    // Control visibility
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    // Add/remove messages
    void AddMessage(const std::string& message, float duration = 3.0f);

    // Set FPS value
    void SetFPS(float fps) { m_fps = fps; }

private:
    // Header bar (top)
    void RenderHeaderBar(int windowWidth);

    // Footer bar (bottom)
    void RenderFooterBar(int windowWidth, int windowHeight);

    // Side message area
    void RenderSideMessages(int windowWidth, int windowHeight);

    // Floating info window
    void RenderFloatingWindow(int windowWidth, int windowHeight);

    // State
    bool m_visible = true;
    float m_fps = 0.0f;

    // Messages with duration
    struct Message {
        std::string text;
        float remainingTime;
    };
    std::vector<Message> m_messages;

    // Floating window state
    bool m_floatingWindowVisible = true;
    ImVec2 m_floatingWindowPos = ImVec2(100, 150);

    // Layout constants
    static constexpr float HEADER_HEIGHT = 38.0f;
    static constexpr float FOOTER_HEIGHT = 24.0f;
    static constexpr float SIDE_WIDTH = 200.0f;
};
