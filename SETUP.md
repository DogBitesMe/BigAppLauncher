# BigAppLauncher 設置說明

## 1. 下載依賴項

在 `external/` 目錄下創建以下子目錄並下載對應的庫：

### ImGui
```bash
cd external
git clone https://github.com/ocornut/imgui.git
```

### nlohmann/json
```bash
cd external
mkdir json
cd json
# 下載 single_include 版本
# 從 https://github.com/nlohmann/json/releases 下載
# 或使用 git:
git clone https://github.com/nlohmann/json.git
```

### stb
```bash
cd external
git clone https://github.com/nothings/stb.git
```

## 2. 下載字體

從 Google Fonts 下載 Noto Sans SC：
https://fonts.google.com/noto/specimen/Noto+Sans+SC

將 `NotoSansSC-Regular.ttf` 放到 `assets/fonts/` 目錄。

## 3. 下載背景影片（可選）

從以下免費資源網站下載一個循環影片：
- https://www.pexels.com/videos/
- https://pixabay.com/videos/
- https://coverr.co/

建議搜索關鍵詞：`abstract`, `technology`, `gaming`, `neon`, `particles`

將影片重命名為 `login_bg.mp4` 並放到 `assets/videos/` 目錄。

## 4. 構建項目

### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### Windows (命令行)

```bash
mkdir build
cd build
cmake .. -G "Ninja"
cmake --build .
```

## 5. 運行

構建完成後，可執行文件位於 `build/bin/` 目錄。

確保 `assets/` 目錄與可執行文件在同一位置（構建時會自動複製）。

## 目錄結構

```
BigAppLauncher/
├── assets/
│   ├── fonts/
│   │   └── NotoSansSC-Regular.ttf    # 需要下載
│   ├── lang/
│   │   ├── zh-CN.json                 # 已提供
│   │   └── en-US.json                 # 已提供
│   └── videos/
│       └── login_bg.mp4              # 需要下載（Phase 2）
├── external/
│   ├── imgui/                         # git clone
│   ├── json/                          # git clone
│   └── stb/                           # git clone
└── src/
    └── ...
```

## 常見問題

### 字體無法載入
如果字體檔案不存在，程序會使用 ImGui 預設字體（不支持中文）。

### 編譯錯誤：找不到 nlohmann/json.hpp
確保 `external/json/include/nlohmann/json.hpp` 路徑正確。

### 編譯錯誤：找不到 imgui.h
確保 `external/imgui/imgui.h` 路徑正確。
