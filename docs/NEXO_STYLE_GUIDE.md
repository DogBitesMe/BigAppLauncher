# NEXO Style UI Design Guide

## Overview

This document describes the design architecture for NEXO-style UI panels used in BigAppLauncher. The design follows a hierarchical box structure with consistent spacing and sizing.

## Window Dimensions

The main application window is designed for **1920x1080** resolution.

| Constant | Value | Description |
|----------|-------|-------------|
| `HEADER_HEIGHT` | 50.0f | Top header bar height |
| `FOOTER_HEIGHT` | 40.0f | Bottom footer bar height |
| `PADDING` | 20.0f | Main content padding from window edges |

**Content Area**: `windowHeight - HEADER_HEIGHT - FOOTER_HEIGHT - PADDING * 2`

## Layout Patterns

### Left-Right Split (Primary Layout)

Used for main tab content (Aimbot, Visual, Misc, Settings tabs).

| Constant | Value | Description |
|----------|-------|-------------|
| `LEFT_PANEL_WIDTH` | 280.0f | Left panel fixed width |
| `rightWidth` | `width - LEFT_PANEL_WIDTH - PADDING` | Right panel calculated width |

```
┌─────────────────────────────────────────────────────────────┐
│ Header (50px)                                               │
├──────────┬──────────────────────────────────────────────────┤
│          │                                                  │
│   Left   │                    Right                         │
│  (280px) │            (remaining width - 20px)              │
│          │                                                  │
│          │                                                  │
├──────────┴──────────────────────────────────────────────────┤
│ Footer (40px)                                               │
└─────────────────────────────────────────────────────────────┘
```

### Three-Column Layout (Sub-content)

Used inside right panel for detailed settings (e.g., Aimbot > General tab).

| Constant | Value | Description |
|----------|-------|-------------|
| `parentPadding` | 12.0f | Padding from parent GroupBox |
| `colSpacing` | 16.0f | Space between columns |
| `colWidth` | `(availWidth - colSpacing * 2) / 3` | Equal column width |

```
┌──────────────────────────────────────────────────────────┐
│ Configuration (Lv1 GroupBox)                             │
│ ┌─────────────┬─────────────┬─────────────┐              │
│ │   Target    │    Speed    │   Keybind   │              │
│ │  (Lv2 Box)  │  (Lv2 Box)  │  (Lv2 Box)  │              │
│ │             │             │             │              │
│ │  [scroll]   │             │             │              │
│ └─────────────┴─────────────┴─────────────┘              │
└──────────────────────────────────────────────────────────┘
```

## GroupBox Hierarchy

### Level 1 (Lv1): GroupBoxFlat

Primary container with lighter background.

| Property | Value | Description |
|----------|-------|-------------|
| Background Color | `rgba(50, 55, 65, 0.5)` | Semi-transparent gray |
| Corner Radius | 10.0f | Large rounded corners |
| Padding X | 12.0f | Horizontal content padding |
| Padding Y | 10.0f | Vertical content padding |

### Level 2 (Lv2): GroupBoxNested

Secondary container with darker background, nested inside Lv1.

| Property | Value | Description |
|----------|-------|-------------|
| Background Color | `rgba(25, 28, 35, 0.8)` | Darker semi-transparent |
| Corner Radius | 8.0f | Slightly smaller corners |
| Padding X | 10.0f | Horizontal content padding |
| Padding Y | 8.0f | Vertical content padding |

## GroupBox Structure

Both Lv1 and Lv2 GroupBoxes use a two-layer structure when fixed height is specified:

```
┌─────────────────────────────────────┐
│ Outer Child (no scroll)             │
│ ┌─────────────────────────────────┐ │
│ │ Header: "Title"                 │ │ ← Fixed position
│ ├─────────────────────────────────┤ │
│ │ Inner Child (scrollable)        │ │
│ │ ┌─────────────────────────────┐ │ │
│ │ │ Content with padding        │ │ │ ← Scrolls independently
│ │ │ ...                         │ │ │
│ │ │ ...                         │ │ │
│ │ └─────────────────────────────┘ │ │
│ └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

- **Outer Child**: Contains header, no scrolling
- **Inner Child**: Contains content with padding, scrollable when content exceeds height

## Tab Styles

### Main Tabs: TabBarPill

Pill-style tabs with rounded backgrounds and animated underline.

- Located in header area
- Centered horizontally using `TabBarPillCalcWidth()`
- Active tab has pulsing underline animation

### Sub Tabs: TabBarButton

Button-style tabs for secondary navigation.

- Located inside GroupBox content area
- Horizontal row of clickable buttons

## Usage Guidelines

1. **Consistent Heights**: When using left-right split, both panels should have equal height (`contentHeight`)

2. **Fixed vs Auto Height**:
   - Use fixed height for scrollable content
   - Use auto height (size.y = 0) for content that fits without scrolling

3. **Nesting Rules**:
   - Lv1 GroupBoxFlat can contain Lv2 GroupBoxNested
   - Avoid deeper nesting for visual clarity

4. **Column Layouts**:
   - Always wrap columns in outer `BeginChild` for proper `SameLine()` behavior
   - Inner GroupBox handles visual styling and scrolling

## File Organization

| File | Purpose |
|------|---------|
| `StyleUI.h` | Public API declarations |
| `StyleUIWidgets.cpp` | Widget implementations (GroupBox, Tabs, etc.) |
| `LargeDemoScreen.cpp` | NEXO-style demo implementation |
| `LargeDemoScreen.h` | Layout constants and state |

## Constants Summary

```cpp
// Window layout (LargeDemoScreen.h)
static constexpr float HEADER_HEIGHT = 50.0f;
static constexpr float FOOTER_HEIGHT = 40.0f;
static constexpr float PADDING = 20.0f;
static constexpr float LEFT_PANEL_WIDTH = 280.0f;

// GroupBoxFlat (Lv1) - StyleUIWidgets.cpp
const float padX = 12.0f;
const float padY = 10.0f;
const float cornerRadius = 10.0f;

// GroupBoxNested (Lv2) - StyleUIWidgets.cpp
const float padX = 10.0f;
const float padY = 8.0f;
const float cornerRadius = 8.0f;

// Three-column layout - LargeDemoScreen.cpp
float parentPadding = 12.0f;
float colSpacing = 16.0f;
```
