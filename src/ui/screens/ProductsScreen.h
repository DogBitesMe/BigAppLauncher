#pragma once

#include <string>
#include <vector>
#include <functional>

class ProductsScreen {
public:
    ProductsScreen();
    ~ProductsScreen() = default;

    // Render the screen
    void Render(int windowWidth, int windowHeight);

    // Update (for animations)
    void Update(float deltaTime);

    // Check if user wants to launch a product
    bool ShouldLaunchProduct() const { return m_shouldLaunch; }
    void ResetLaunch() { m_shouldLaunch = false; }

    // Window controls callback
    using WindowControlCallback = std::function<void(int action)>;  // 0=minimize, 1=close
    void SetWindowControlCallback(WindowControlCallback callback) { m_windowControlCallback = callback; }

private:
    // Sidebar navigation
    enum class SidebarTab {
        Home = 0,
        Product1,
        Product2,
        Dummy1,
        Dummy2
    };
    SidebarTab m_currentTab = SidebarTab::Home;

    // Tab content renderers
    void RenderSidebar(float width, float height);
    void RenderContent(float x, float width, float height);
    void RenderAccountBar(float width);
    void RenderHomeTab(float width, float height);
    void RenderProductTab(int productIndex, float width, float height);

    // Home tab - Carousel
    struct CarouselState {
        int currentSlide = 0;
        float autoTimer = 0.0f;
        float manualPauseTimer = 0.0f;
        bool isPaused = false;
        static constexpr float AUTO_INTERVAL = 5.0f;
        static constexpr float MANUAL_PAUSE_DURATION = 20.0f;
        static constexpr int SLIDE_COUNT = 4;
    };
    CarouselState m_carousel;

    void RenderCarousel(float width, float height);
    void RenderAnnouncements(float width, float height);

    // Product tab
    void RenderProductBanner(float width, float height);
    void RenderProductUpdates(float width, float height);
    void RenderProductActions(float width, float height);

    // State
    bool m_shouldLaunch = false;
    int m_selectedVersion = 0;
    int m_selectedRegion = 0;  // 0 = CN, 1 = International
    std::string m_username = "TestUser";

    // Window controls
    WindowControlCallback m_windowControlCallback;

    // Sidebar config
    static constexpr float SIDEBAR_WIDTH = 70.0f;
    static constexpr float ACCOUNT_BAR_HEIGHT = 50.0f;
};
