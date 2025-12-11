# Main application sources
set(SOURCES
    main.cpp
    Application.cpp

    # Core
    core/Config.cpp
    core/Logger.cpp

    # Graphics
    graphics/DX11Context.cpp
    graphics/TextureManager.cpp
    graphics/VideoPlayer.cpp
    graphics/BlurEffect.cpp

    # UI
    ui/Theme.cpp
    ui/Widgets.cpp
    ui/DebugController.cpp
    ui/HUDOverlay.cpp
    ui/screens/LoginScreen.cpp
    ui/screens/ProductsScreen.cpp
    ui/screens/GUIMenuScreen.cpp

    # i18n
    i18n/Localization.cpp
)

set(HEADERS
    Application.h

    # Core
    core/Config.h
    core/Logger.h

    # Graphics
    graphics/DX11Context.h
    graphics/TextureManager.h
    graphics/VideoPlayer.h
    graphics/BlurEffect.h

    # UI
    ui/Theme.h
    ui/Widgets.h
    ui/DebugController.h
    ui/HUDOverlay.h
    ui/screens/LoginScreen.h
    ui/screens/ProductsScreen.h
    ui/screens/GUIMenuScreen.h

    # i18n
    i18n/Localization.h
)

add_executable(BigAppLauncher WIN32 ${SOURCES} ${HEADERS})

target_include_directories(BigAppLauncher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(BigAppLauncher PRIVATE
    imgui
    json
    stb
    d3d11
    dxgi
    d3dcompiler
    mfplat
    mfreadwrite
    mfuuid
    shlwapi
)

# Enable warnings
if(MSVC)
    target_compile_options(BigAppLauncher PRIVATE /W4)
endif()

# Copy assets to output directory
add_custom_command(TARGET BigAppLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:BigAppLauncher>/assets
)
