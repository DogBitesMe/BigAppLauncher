# BigAppLauncher Dependency Setup Script
# Run this script in PowerShell to download all dependencies

$ErrorActionPreference = "Stop"

Write-Host "Setting up BigAppLauncher dependencies..." -ForegroundColor Cyan

# Create external directory
if (!(Test-Path "external")) {
    New-Item -ItemType Directory -Path "external" | Out-Null
}

Set-Location "external"

# Clone ImGui
if (!(Test-Path "imgui")) {
    Write-Host "Downloading ImGui..." -ForegroundColor Yellow
    git clone --depth 1 https://github.com/ocornut/imgui.git
} else {
    Write-Host "ImGui already exists, skipping..." -ForegroundColor Green
}

# Clone nlohmann/json
if (!(Test-Path "json")) {
    Write-Host "Downloading nlohmann/json..." -ForegroundColor Yellow
    git clone --depth 1 https://github.com/nlohmann/json.git
} else {
    Write-Host "nlohmann/json already exists, skipping..." -ForegroundColor Green
}

# Clone stb
if (!(Test-Path "stb")) {
    Write-Host "Downloading stb..." -ForegroundColor Yellow
    git clone --depth 1 https://github.com/nothings/stb.git
} else {
    Write-Host "stb already exists, skipping..." -ForegroundColor Green
}

Set-Location ".."

# Create asset directories
Write-Host "Creating asset directories..." -ForegroundColor Yellow

$dirs = @(
    "assets/fonts",
    "assets/videos",
    "assets/images"
)

foreach ($dir in $dirs) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
}

# Download Noto Sans SC font
$fontPath = "assets/fonts/NotoSansSC-Regular.ttf"
if (!(Test-Path $fontPath)) {
    Write-Host "Downloading Noto Sans SC font..." -ForegroundColor Yellow
    $fontUrl = "https://github.com/googlefonts/noto-cjk/raw/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf"
    try {
        Invoke-WebRequest -Uri $fontUrl -OutFile "assets/fonts/NotoSansSC-Regular.otf"
        Write-Host "Font downloaded (as .otf)" -ForegroundColor Green
    } catch {
        Write-Host "Could not download font automatically." -ForegroundColor Red
        Write-Host "Please download manually from: https://fonts.google.com/noto/specimen/Noto+Sans+SC" -ForegroundColor Yellow
    }
} else {
    Write-Host "Font already exists, skipping..." -ForegroundColor Green
}

Write-Host ""
Write-Host "Setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. (Optional) Download a background video to assets/videos/login_bg.mp4"
Write-Host "2. Run: mkdir build && cd build && cmake .. && cmake --build ."
Write-Host ""
