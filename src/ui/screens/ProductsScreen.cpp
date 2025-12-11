#include "ProductsScreen.h"
#include "../Theme.h"
#include "../Widgets.h"
#include "../IconsFontAwesome6.h"
#include "../../i18n/Localization.h"
#include <imgui.h>

ProductsScreen::ProductsScreen() {
}

void ProductsScreen::Update(float deltaTime) {
    // Update carousel auto-rotation
    if (m_carousel.isPaused) {
        m_carousel.manualPauseTimer -= deltaTime;
        if (m_carousel.manualPauseTimer <= 0.0f) {
            m_carousel.isPaused = false;
        }
    } else {
        m_carousel.autoTimer += deltaTime;
        if (m_carousel.autoTimer >= CarouselState::AUTO_INTERVAL) {
            m_carousel.autoTimer = 0.0f;
            m_carousel.currentSlide = (m_carousel.currentSlide + 1) % CarouselState::SLIDE_COUNT;
        }
    }
}

void ProductsScreen::Render(int windowWidth, int windowHeight) {
    float width = static_cast<float>(windowWidth);
    float height = static_cast<float>(windowHeight);

    // Render sidebar
    RenderSidebar(SIDEBAR_WIDTH, height);

    // Render main content area
    float contentX = SIDEBAR_WIDTH;
    float contentWidth = width - SIDEBAR_WIDTH;
    RenderContent(contentX, contentWidth, height);
}

void ProductsScreen::RenderSidebar(float width, float height) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.12f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (ImGui::Begin("##Sidebar", nullptr, flags)) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();

        // Product icon at top (placeholder)
        float iconSize = 50.0f;
        float iconX = (width - iconSize) * 0.5f;
        float iconY = 15.0f;

        drawList->AddRectFilled(
            ImVec2(windowPos.x + iconX, windowPos.y + iconY),
            ImVec2(windowPos.x + iconX + iconSize, windowPos.y + iconY + iconSize),
            IM_COL32(66, 150, 255, 255),
            8.0f
        );

        // Draw "B" letter as placeholder
        ImVec2 textPos(windowPos.x + iconX + 15, windowPos.y + iconY + 10);
        drawList->AddText(ImGui::GetFont(), 28.0f, textPos, IM_COL32(255, 255, 255, 255), "B");

        // Navigation tabs
        float tabY = iconY + iconSize + 20.0f;
        float tabHeight = 50.0f;
        float tabSpacing = 5.0f;

        struct TabInfo {
            const char* icon;
            const char* tooltip;
            SidebarTab tab;
        };

        TabInfo tabs[] = {
            { ICON_FA_HOUSE, "Home", SidebarTab::Home },
            { ICON_FA_GAMEPAD, "Product 1", SidebarTab::Product1 },
            { ICON_FA_PUZZLE_PIECE, "Product 2", SidebarTab::Product2 },
            { ICON_FA_CUBE, "Dummy 1", SidebarTab::Dummy1 },
            { ICON_FA_CUBES, "Dummy 2", SidebarTab::Dummy2 },
        };

        for (int i = 0; i < 5; i++) {
            ImVec2 tabPos(windowPos.x + 5, windowPos.y + tabY + i * (tabHeight + tabSpacing));
            ImVec2 tabSize(width - 10, tabHeight);

            bool isActive = (m_currentTab == tabs[i].tab);
            bool isHovered = ImGui::IsMouseHoveringRect(tabPos, ImVec2(tabPos.x + tabSize.x, tabPos.y + tabSize.y));

            // Tab background
            ImU32 bgColor = isActive ? IM_COL32(66, 150, 255, 80) :
                           (isHovered ? IM_COL32(255, 255, 255, 20) : IM_COL32(0, 0, 0, 0));

            if (isActive || isHovered) {
                drawList->AddRectFilled(tabPos, ImVec2(tabPos.x + tabSize.x, tabPos.y + tabSize.y), bgColor, 8.0f);
            }

            // Active indicator
            if (isActive) {
                drawList->AddRectFilled(
                    ImVec2(tabPos.x, tabPos.y + 10),
                    ImVec2(tabPos.x + 3, tabPos.y + tabSize.y - 10),
                    IM_COL32(66, 150, 255, 255),
                    2.0f
                );
            }

            // Icon
            ImVec2 iconTextSize = ImGui::CalcTextSize(tabs[i].icon);
            ImVec2 iconPos(
                tabPos.x + (tabSize.x - iconTextSize.x) * 0.5f,
                tabPos.y + (tabSize.y - iconTextSize.y) * 0.5f
            );

            ImU32 iconColor = isActive ? IM_COL32(66, 150, 255, 255) : IM_COL32(180, 180, 180, 255);
            drawList->AddText(iconPos, iconColor, tabs[i].icon);

            // Handle click
            if (isHovered && ImGui::IsMouseClicked(0)) {
                m_currentTab = tabs[i].tab;
            }

            // Tooltip
            if (isHovered) {
                ImGui::SetTooltip("%s", tabs[i].tooltip);
            }
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void ProductsScreen::RenderContent(float x, float width, float height) {
    ImGui::SetNextWindowPos(ImVec2(x, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.09f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (ImGui::Begin("##Content", nullptr, flags)) {
        // Account bar at top
        RenderAccountBar(width);

        // Content below account bar
        float contentY = ACCOUNT_BAR_HEIGHT;
        float contentHeight = height - ACCOUNT_BAR_HEIGHT;

        ImGui::SetCursorPos(ImVec2(0, contentY));

        switch (m_currentTab) {
            case SidebarTab::Home:
                RenderHomeTab(width, contentHeight);
                break;
            case SidebarTab::Product1:
                RenderProductTab(0, width, contentHeight);
                break;
            case SidebarTab::Product2:
                RenderProductTab(1, width, contentHeight);
                break;
            case SidebarTab::Dummy1:
            case SidebarTab::Dummy2:
                // Placeholder for dummy tabs
                ImGui::SetCursorPos(ImVec2(width * 0.5f - 100, contentY + contentHeight * 0.5f - 20));
                ImGui::Text("Coming Soon...");
                break;
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void ProductsScreen::RenderAccountBar(float width) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();

    // Background
    drawList->AddRectFilled(
        windowPos,
        ImVec2(windowPos.x + width, windowPos.y + ACCOUNT_BAR_HEIGHT),
        IM_COL32(20, 20, 28, 255)
    );

    // User info
    float avatarSize = 32.0f;
    float avatarX = 15.0f;
    float avatarY = (ACCOUNT_BAR_HEIGHT - avatarSize) * 0.5f;

    // Avatar placeholder (circle)
    ImVec2 avatarCenter(windowPos.x + avatarX + avatarSize * 0.5f, windowPos.y + avatarY + avatarSize * 0.5f);
    drawList->AddCircleFilled(avatarCenter, avatarSize * 0.5f, IM_COL32(66, 150, 255, 255));
    drawList->AddText(
        ImVec2(avatarCenter.x - 5, avatarCenter.y - 8),
        IM_COL32(255, 255, 255, 255),
        "U"
    );

    // Username
    float textX = avatarX + avatarSize + 10.0f;
    float textY = (ACCOUNT_BAR_HEIGHT - 16.0f) * 0.5f;
    drawList->AddText(
        ImVec2(windowPos.x + textX, windowPos.y + textY),
        IM_COL32(220, 220, 220, 255),
        m_username.c_str()
    );

    // Window controls (right side)
    float buttonSize = 32.0f;
    float buttonSpacing = 4.0f;
    float buttonY = (ACCOUNT_BAR_HEIGHT - buttonSize) * 0.5f;

    // Close button
    float closeX = width - buttonSize - 10.0f;
    ImVec2 closePos(windowPos.x + closeX, windowPos.y + buttonY);
    bool closeHovered = ImGui::IsMouseHoveringRect(closePos, ImVec2(closePos.x + buttonSize, closePos.y + buttonSize));

    drawList->AddRectFilled(
        closePos,
        ImVec2(closePos.x + buttonSize, closePos.y + buttonSize),
        closeHovered ? IM_COL32(200, 60, 60, 255) : IM_COL32(60, 60, 70, 200),
        6.0f
    );

    ImVec2 closeIconSize = ImGui::CalcTextSize(ICON_FA_XMARK);
    drawList->AddText(
        ImVec2(closePos.x + (buttonSize - closeIconSize.x) * 0.5f, closePos.y + (buttonSize - closeIconSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        ICON_FA_XMARK
    );

    if (closeHovered && ImGui::IsMouseClicked(0) && m_windowControlCallback) {
        m_windowControlCallback(1);  // Close
    }

    // Minimize button
    float minX = closeX - buttonSize - buttonSpacing;
    ImVec2 minPos(windowPos.x + minX, windowPos.y + buttonY);
    bool minHovered = ImGui::IsMouseHoveringRect(minPos, ImVec2(minPos.x + buttonSize, minPos.y + buttonSize));

    drawList->AddRectFilled(
        minPos,
        ImVec2(minPos.x + buttonSize, minPos.y + buttonSize),
        minHovered ? IM_COL32(80, 80, 90, 255) : IM_COL32(60, 60, 70, 200),
        6.0f
    );

    ImVec2 minIconSize = ImGui::CalcTextSize(ICON_FA_WINDOW_MINIMIZE);
    drawList->AddText(
        ImVec2(minPos.x + (buttonSize - minIconSize.x) * 0.5f, minPos.y + (buttonSize - minIconSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        ICON_FA_WINDOW_MINIMIZE
    );

    if (minHovered && ImGui::IsMouseClicked(0) && m_windowControlCallback) {
        m_windowControlCallback(0);  // Minimize
    }
}

void ProductsScreen::RenderHomeTab(float width, float height) {
    // Carousel takes ~55% of height
    float carouselHeight = height * 0.55f;
    RenderCarousel(width, carouselHeight);

    // Announcements below
    ImGui::SetCursorPos(ImVec2(0, ACCOUNT_BAR_HEIGHT + carouselHeight));
    RenderAnnouncements(width, height - carouselHeight);
}

void ProductsScreen::RenderCarousel(float width, float height) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    float startY = ACCOUNT_BAR_HEIGHT;

    // Carousel background colors for each slide
    ImU32 slideColors[] = {
        IM_COL32(40, 80, 120, 255),   // Blue
        IM_COL32(80, 40, 100, 255),   // Purple
        IM_COL32(40, 100, 80, 255),   // Teal
        IM_COL32(100, 60, 40, 255),   // Orange
    };

    float padding = 20.0f;
    ImVec2 slidePos(windowPos.x + padding, windowPos.y + startY + padding);
    ImVec2 slideSize(width - padding * 2, height - padding * 2 - 30);  // 30 for dots

    // Draw current slide
    drawList->AddRectFilled(
        slidePos,
        ImVec2(slidePos.x + slideSize.x, slidePos.y + slideSize.y),
        slideColors[m_carousel.currentSlide],
        12.0f
    );

    // Slide content (placeholder text)
    const char* slideTexts[] = {
        "Welcome to BigAppLauncher!",
        "New Features Available",
        "Special Promotion",
        "Community News"
    };

    ImVec2 textSize = ImGui::CalcTextSize(slideTexts[m_carousel.currentSlide]);
    drawList->AddText(
        ImGui::GetFont(),
        24.0f,
        ImVec2(slidePos.x + (slideSize.x - textSize.x * 1.3f) * 0.5f, slidePos.y + slideSize.y * 0.4f),
        IM_COL32(255, 255, 255, 255),
        slideTexts[m_carousel.currentSlide]
    );

    // Navigation arrows
    float arrowSize = 40.0f;
    float arrowY = slidePos.y + (slideSize.y - arrowSize) * 0.5f;

    // Left arrow
    ImVec2 leftArrowPos(slidePos.x + 10, arrowY);
    bool leftHovered = ImGui::IsMouseHoveringRect(leftArrowPos, ImVec2(leftArrowPos.x + arrowSize, leftArrowPos.y + arrowSize));

    drawList->AddRectFilled(
        leftArrowPos,
        ImVec2(leftArrowPos.x + arrowSize, leftArrowPos.y + arrowSize),
        leftHovered ? IM_COL32(0, 0, 0, 150) : IM_COL32(0, 0, 0, 80),
        arrowSize * 0.5f
    );

    ImVec2 leftIconSize = ImGui::CalcTextSize(ICON_FA_CHEVRON_LEFT);
    drawList->AddText(
        ImVec2(leftArrowPos.x + (arrowSize - leftIconSize.x) * 0.5f, leftArrowPos.y + (arrowSize - leftIconSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 200),
        ICON_FA_CHEVRON_LEFT
    );

    if (leftHovered && ImGui::IsMouseClicked(0)) {
        m_carousel.currentSlide = (m_carousel.currentSlide - 1 + CarouselState::SLIDE_COUNT) % CarouselState::SLIDE_COUNT;
        m_carousel.isPaused = true;
        m_carousel.manualPauseTimer = CarouselState::MANUAL_PAUSE_DURATION;
    }

    // Right arrow
    ImVec2 rightArrowPos(slidePos.x + slideSize.x - arrowSize - 10, arrowY);
    bool rightHovered = ImGui::IsMouseHoveringRect(rightArrowPos, ImVec2(rightArrowPos.x + arrowSize, rightArrowPos.y + arrowSize));

    drawList->AddRectFilled(
        rightArrowPos,
        ImVec2(rightArrowPos.x + arrowSize, rightArrowPos.y + arrowSize),
        rightHovered ? IM_COL32(0, 0, 0, 150) : IM_COL32(0, 0, 0, 80),
        arrowSize * 0.5f
    );

    ImVec2 rightIconSize = ImGui::CalcTextSize(ICON_FA_CHEVRON_RIGHT);
    drawList->AddText(
        ImVec2(rightArrowPos.x + (arrowSize - rightIconSize.x) * 0.5f, rightArrowPos.y + (arrowSize - rightIconSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 200),
        ICON_FA_CHEVRON_RIGHT
    );

    if (rightHovered && ImGui::IsMouseClicked(0)) {
        m_carousel.currentSlide = (m_carousel.currentSlide + 1) % CarouselState::SLIDE_COUNT;
        m_carousel.isPaused = true;
        m_carousel.manualPauseTimer = CarouselState::MANUAL_PAUSE_DURATION;
    }

    // Dot indicators
    float dotSize = 8.0f;
    float dotSpacing = 12.0f;
    float dotsWidth = CarouselState::SLIDE_COUNT * dotSize + (CarouselState::SLIDE_COUNT - 1) * dotSpacing;
    float dotsX = slidePos.x + (slideSize.x - dotsWidth) * 0.5f;
    float dotsY = slidePos.y + slideSize.y + 10;

    for (int i = 0; i < CarouselState::SLIDE_COUNT; i++) {
        ImVec2 dotPos(dotsX + i * (dotSize + dotSpacing), dotsY);
        bool isActive = (i == m_carousel.currentSlide);
        bool dotHovered = ImGui::IsMouseHoveringRect(dotPos, ImVec2(dotPos.x + dotSize, dotPos.y + dotSize));

        drawList->AddCircleFilled(
            ImVec2(dotPos.x + dotSize * 0.5f, dotPos.y + dotSize * 0.5f),
            dotSize * 0.5f,
            isActive ? IM_COL32(66, 150, 255, 255) : (dotHovered ? IM_COL32(150, 150, 150, 255) : IM_COL32(100, 100, 100, 255))
        );

        if (dotHovered && ImGui::IsMouseClicked(0)) {
            m_carousel.currentSlide = i;
            m_carousel.isPaused = true;
            m_carousel.manualPauseTimer = CarouselState::MANUAL_PAUSE_DURATION;
        }
    }
}

void ProductsScreen::RenderAnnouncements(float width, float height) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.11f, 1.0f));

    ImGui::BeginChild("##Announcements", ImVec2(width, height), false);

    ImGui::SetCursorPos(ImVec2(20, 15));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::Text("%s %s", ICON_FA_BULLHORN, "Announcements");
    ImGui::PopStyleColor();

    ImGui::SetCursorPosX(20);
    ImGui::Separator();

    ImGui::SetCursorPos(ImVec2(20, 50));

    // Sample announcements
    const char* announcements[] = {
        "[Notice] Server maintenance scheduled for next week",
        "[Update] Version 2.0 now available with new features",
        "[Event] Double XP weekend starting Friday!",
        "[Info] New payment methods now supported",
        "[Community] Share your feedback and win prizes"
    };

    for (int i = 0; i < 5; i++) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("%s", announcements[i]);
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}

void ProductsScreen::RenderProductTab(int productIndex, float width, float height) {
    // Product background color
    ImU32 bgColors[] = {
        IM_COL32(25, 35, 50, 255),  // Product 1 - Dark blue
        IM_COL32(35, 25, 45, 255),  // Product 2 - Dark purple
    };

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();

    // Background
    drawList->AddRectFilled(
        ImVec2(windowPos.x, windowPos.y + ACCOUNT_BAR_HEIGHT),
        ImVec2(windowPos.x + width, windowPos.y + ACCOUNT_BAR_HEIGHT + height),
        bgColors[productIndex % 2]
    );

    // Banner (1/3 height)
    float bannerHeight = height * 0.33f;
    RenderProductBanner(width, bannerHeight);

    // Content below banner
    float contentY = ACCOUNT_BAR_HEIGHT + bannerHeight;
    float contentHeight = height - bannerHeight;

    // Two columns: Updates (left) and Actions (right)
    float padding = 20.0f;
    float columnGap = 20.0f;
    float actionsWidth = 280.0f;
    float updatesWidth = width - actionsWidth - padding * 2 - columnGap;

    ImGui::SetCursorPos(ImVec2(padding, contentY + padding));
    ImGui::BeginChild("##Updates", ImVec2(updatesWidth, contentHeight - padding * 2), false);
    RenderProductUpdates(updatesWidth, contentHeight - padding * 2);
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(padding + updatesWidth + columnGap, contentY + padding));
    ImGui::BeginChild("##Actions", ImVec2(actionsWidth, contentHeight - padding * 2), false);
    RenderProductActions(actionsWidth, contentHeight - padding * 2);
    ImGui::EndChild();
}

void ProductsScreen::RenderProductBanner(float width, float height) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    float startY = ACCOUNT_BAR_HEIGHT;

    // Banner gradient
    drawList->AddRectFilledMultiColor(
        ImVec2(windowPos.x, windowPos.y + startY),
        ImVec2(windowPos.x + width, windowPos.y + startY + height),
        IM_COL32(60, 100, 140, 255),
        IM_COL32(40, 70, 100, 255),
        IM_COL32(30, 50, 80, 255),
        IM_COL32(50, 80, 120, 255)
    );

    // Banner title
    const char* title = "Update Information";
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    drawList->AddText(
        ImGui::GetFont(),
        28.0f,
        ImVec2(windowPos.x + 30, windowPos.y + startY + height * 0.4f),
        IM_COL32(255, 255, 255, 255),
        title
    );

    // Subtitle
    const char* subtitle = "v2.1.0 - Latest Release";
    drawList->AddText(
        ImVec2(windowPos.x + 30, windowPos.y + startY + height * 0.4f + 35),
        IM_COL32(200, 200, 200, 200),
        subtitle
    );
}

void ProductsScreen::RenderProductUpdates(float width, float height) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.14f, 0.8f));

    ImGui::BeginChild("##UpdatesContent", ImVec2(width, height), false, ImGuiWindowFlags_NoScrollbar);

    ImGui::SetCursorPos(ImVec2(15, 15));
    ImGui::Text("%s Update Log", ICON_FA_CLOCK_ROTATE_LEFT);

    ImGui::SetCursorPosX(15);
    ImGui::Separator();

    ImGui::SetCursorPos(ImVec2(15, 45));

    // Sample update log
    const char* updates[] = {
        "- Fixed crash when loading certain maps",
        "- Improved performance on low-end devices",
        "- Added new customization options",
        "- Updated anti-cheat system",
        "- Fixed various UI bugs"
    };

    for (int i = 0; i < 5; i++) {
        ImGui::TextWrapped("%s", updates[i]);
        ImGui::Spacing();
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}

void ProductsScreen::RenderProductActions(float width, float height) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.14f, 0.8f));

    ImGui::BeginChild("##ActionsContent", ImVec2(width, height), false, ImGuiWindowFlags_NoScrollbar);

    float padding = 15.0f;
    float buttonWidth = width - padding * 2;

    ImGui::SetCursorPos(ImVec2(padding, padding));

    // Launch button
    if (Widgets::GradientButton("Launch Game", ImVec2(buttonWidth, 45))) {
        m_shouldLaunch = true;
    }

    ImGui::SetCursorPosX(padding);
    ImGui::Spacing();
    ImGui::Spacing();

    // Version selector
    ImGui::SetCursorPosX(padding);
    ImGui::Text("Version:");
    ImGui::SetCursorPosX(padding);
    ImGui::PushItemWidth(buttonWidth);

    const char* versions[] = { "v2.1.0 (Latest)", "v2.0.5", "v2.0.0", "v1.9.8" };
    ImGui::Combo("##Version", &m_selectedVersion, versions, 4);

    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();

    // Region selector
    ImGui::SetCursorPosX(padding);
    ImGui::Text("Region:");
    ImGui::SetCursorPosX(padding);
    ImGui::PushItemWidth(buttonWidth);

    const char* regions[] = { "CN Server", "International" };
    ImGui::Combo("##Region", &m_selectedRegion, regions, 2);

    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();

    // Update button
    ImGui::SetCursorPosX(padding);
    if (ImGui::Button("Check for Updates", ImVec2(buttonWidth, 35))) {
        // Check update logic
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}
