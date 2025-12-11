# BigAppLauncher - Claude Code Project Guide

## Project Overview

BigAppLauncher is a WeGame-style product launcher application built with C++17, DirectX 11, and ImGui. It features a video background with frosted glass effects, multi-screen navigation, and a modern UI design.

## Tech Stack

- **Language**: C++17
- **Graphics**: DirectX 11
- **UI Framework**: Dear ImGui
- **Video**: Media Foundation (Windows)
- **JSON**: nlohmann/json
- **Icons**: FontAwesome 6 (via IconFontCppHeaders)
- **Build**: CMake 3.20+

## Project Structure

```
BigAppLauncher/
├── assets/
│   ├── fonts/
│   │   ├── NotoSansSC-Regular.otf    # Chinese font
│   │   └── fa-solid-900.ttf          # FontAwesome icons
│   ├── lang/
│   │   ├── zh-CN.json                # Simplified Chinese
│   │   ├── zh-TW.json                # Traditional Chinese
│   │   └── en-US.json                # English
│   └── videos/
│       └── login_bg.mp4              # Background video
├── external/
│   ├── imgui/                        # Dear ImGui library
│   ├── json/                         # nlohmann/json
│   └── stb/                          # stb_image
├── src/
│   ├── Application.cpp/h             # Main application controller
│   ├── main.cpp                      # Entry point
│   ├── core/
│   │   ├── Config.cpp/h              # Configuration management
│   │   └── Logger.cpp/h              # Logging system
│   ├── graphics/
│   │   ├── DX11Context.cpp/h         # DirectX 11 context
│   │   ├── VideoPlayer.cpp/h         # Media Foundation video
│   │   ├── BlurEffect.cpp/h          # Gaussian blur shader
│   │   └── TextureManager.cpp/h      # Texture loading
│   ├── ui/
│   │   ├── Theme.cpp/h               # Color palette and styling
│   │   ├── Widgets.cpp/h             # Reusable UI components
│   │   ├── IconsFontAwesome6.h       # Icon definitions
│   │   ├── DebugController.cpp/h     # Debug test panel
│   │   ├── HUDOverlay.cpp/h          # HUD overlay system
│   │   └── screens/
│   │       ├── LoginScreen.cpp/h     # Login UI
│   │       ├── ProductsScreen.cpp/h  # Products page (WeGame-style)
│   │       └── GUIMenuScreen.cpp/h   # Settings menu
│   └── i18n/
│       └── Localization.cpp/h        # Multi-language support
└── CMakeLists.txt
```

## Application States

```
Login → (success + 1s delay) → Products → (Launch) → GUIMenu
                                        ← (Close)
```

- **Login**: Card key or account login with video background and blur effect
- **Products**: WeGame-style layout with sidebar, carousel, product tabs
- **GUIMenu**: Settings menu with HUD overlay

## Key Components

### Application (`src/Application.cpp`)
- Window creation (borderless with rounded corners)
- State management (Login → Products → GUIMenu)
- Render loop coordination
- DWM integration for Windows 11 effects

### LoginScreen (`src/ui/screens/LoginScreen.cpp`)
- Tab-based login (CardKey/Account/Register/Password)
- Configurable login mode and result via DebugController
- Error messages: connection error, wrong credentials, expired
- Success delay before transition

### ProductsScreen (`src/ui/screens/ProductsScreen.cpp`)
- Left sidebar with vertical icon navigation
- Account info bar with window controls
- Home tab: Image carousel (auto/manual rotation)
- Product tabs: Banner, update log, action panel

### GUIMenuScreen (`src/ui/screens/GUIMenuScreen.cpp`)
- Tab pages: Visual, Items, Radar, General
- UI component showcase: checkboxes, sliders, radio buttons, color picker

### HUDOverlay (`src/ui/HUDOverlay.cpp`)
- Header bar: FPS display, status indicators
- Footer bar: Instructions/help text
- Side messages: Notifications with fade effect
- Floating window: Draggable statistics panel

### Theme (`src/ui/Theme.cpp`)
Key colors:
- Primary: `#4296FF` (blue)
- Background: `#0F0F17` (dark)
- Success: `#42C769` (green)
- Error: `#FA5C5C` (red)

### Widgets (`src/ui/Widgets.cpp`)
- `GradientButton()` - Gradient-filled button
- `InputText()` / `InputPassword()` - Styled inputs
- `TabBar()` - Horizontal tabs
- `MessageBox()` - Status messages
- `LanguageSelector()` - Language dropdown
- `Spinner()` - Loading animation

## Localization

Uses JSON files with dot-notation keys:
```json
{
  "login": {
    "tab_cardkey": "Card Key",
    "error_connection": "Connection error"
  }
}
```

Access via: `i18n::T("login.tab_cardkey")`

## Build Commands

```bash
# Configure
cmake -B build -G "Visual Studio 17 2022"

# Build Release
cmake --build build --config Release

# Run
build/bin/Release/BigAppLauncher.exe
```

## Debug Controller

Press F12 or look for the Debug Controller window to:
- Toggle between CardKey/Account login modes
- Set login result (Success, Connection Error, Wrong Credentials, Expired)
- Enable/disable skip entry page

## Important Notes

1. **Video playback**: RGB32 format from Media Foundation has alpha=0, forced to 255 in VideoPlayer
2. **Borderless window**: Uses WS_POPUP with custom WM_NCHITTEST for dragging/resizing
3. **Blur effect**: Two-pass Gaussian blur using ping-pong render targets
4. **Icon centering**: Custom rendering with CalcTextSize for proper icon alignment
5. **Transparency**: HUD elements use black (0,0,0) as transparent for overlay compositing

## File Naming Conventions

- Classes: PascalCase (`LoginScreen`, `VideoPlayer`)
- Files: Match class name (`LoginScreen.cpp`)
- Headers: `.h` extension
- Members: `m_` prefix (`m_currentTab`, `m_isLoggedIn`)

## Common Tasks

### Adding a new screen
1. Create `src/ui/screens/NewScreen.h/cpp`
2. Add to `CMakeLists.txt`
3. Add state to `AppState` enum in `Application.h`
4. Add unique_ptr member and instantiation
5. Add render case in `Application::Render()`

### Adding a new widget
1. Declare in `src/ui/Widgets.h`
2. Implement in `src/ui/Widgets.cpp`
3. Use Theme colors and sizes for consistency

### Adding localization strings
1. Add key to all JSON files in `assets/lang/`
2. Access via `i18n::T("key.subkey")`
