# Montre Smart — M5StickC-Plus2 Firmware

## Project Overview

PlatformIO/Arduino firmware for **M5StickC-Plus2** (ESP32-based smartwatch). C++ codebase. No host-side build tools beyond PlatformIO.

## Build Commands

```bash
# Build
pio run

# Upload to device
pio run -t upload

# Serial monitor
pio device monitor --baud 115200

# Build + upload + monitor (one shot)
pio run -t upload && pio device monitor --baud 115200

# Run native unit tests (no device needed)
pio test -e native
```

## Architecture

### Naming Conventions
| Suffix | Role |
|--------|------|
| `*_manager.h` | High-level orchestrator (page nav, menu stack) |
| `*_handler.h` | Focused reusable component (mini-library) |
| `*_utils.h` | Stateless utility functions |

### Core Systems
- **PageBase** (`lib/pages/page_base.h`) — base class all pages inherit. Handles button routing, menu lifecycle.
- **PageManager** (`lib/page_manager.h`) — registers pages, drives `setup()`/`loop()`/`cleanup()` transitions.
- **MenuManager** (`lib/menu_manager.h`) — LIFO stack of `MenuHandler` instances.
- **MenuHandler** (`lib/menu_handler.h`) — individual menu with items + callbacks.
- **SettingsManager** (`lib/settings_manager.h`) — singleton, caches NVS settings in RAM (Preferences reads are slow).
- **DisplayHandler** (`lib/display_handler.h`) — semantic display zones + message types.
- **BatteryHandler** (`lib/battery_handler.h`) — power management, deep sleep.
- **ClockHandler** (`lib/clock_handler.h`) — RTC time/date, Pomodoro.
- **TimeSelector** (`lib/time_selector.h`) — reusable picker widget (seconds / HH:MM / HH:MM:SS).

### main.cpp Pattern
```cpp
// Globals: handlers + manager + settings
// setup(): init settings, handlers, pages, pageManager.begin()
// loop(): M5.update() → pageManager.handleInput() → pageManager.update() → batteryHandler.update() → sleep check
```

## Critical Rules

### Button Detection
**NEVER use `wasPressed()` for PWR or B buttons** — they get stuck internally.
Always use manual rising-edge detection (already in `PageBase::handleBasicInputInteractions()`):

```cpp
bool currentPWRState = M5.BtnPWR.isPressed();
if (currentPWRState && !lastBtnPWRState) { /* rising edge */ }
lastBtnPWRState = currentPWRState;
```

Button A uses `wasPressed()` — that one works fine.

### Deep Sleep / GPIO4
GPIO4 must stay HIGH during deep sleep or device loses power immediately:
```cpp
pinMode(4, OUTPUT);
digitalWrite(4, HIGH);
gpio_hold_en(GPIO_NUM_4);
gpio_deep_sleep_hold_en();
```
This is handled in `BatteryHandler::M5deepSleep()` — don't reimplement.

### SettingsManager
Singleton. Access anywhere via `SettingsManager::getInstance()`. Cache loaded once at `begin()` — setters persist to NVS immediately.

## File Structure

```
src/
  main.cpp                  # Entry point
lib/
  display_handler.h
  clock_handler.h
  battery_handler.h
  button_handler.h
  menu_handler.h
  menu_manager.h
  page_manager.h
  rtc_utils.h
  settings_manager.h
  time_selector.h
  wifi_helper.h             # ⚠️ untested in this project
  mqtt_helper.h             # ⚠️ untested, needs AsyncMqttClient + ArduinoJson
  pages/
    page_base.h             # Base class
    clock_page.h            # Default clock page
test/
  test_embedded/            # On-device tests
platformio.ini
```

## Creating a New Page

1. Create `lib/pages/your_page.h` inheriting `PageBase`
2. Override `setup()`, `loop()`, `getName()`
3. Override button handlers as needed (all have defaults in PageBase)
4. Call `handleBasicInputInteractions()` inside `handleInput()` override
5. Register in `main.cpp`:

```cpp
#include "../lib/pages/your_page.h"
YourPage* yourPage = new YourPage(&displayHandler);
pageManager.addPage(yourPage);
```

Minimal template:
```cpp
#ifndef YOUR_PAGE_H
#define YOUR_PAGE_H
#include "page_base.h"

class YourPage : public PageBase {
public:
    YourPage(DisplayHandler* disp) : PageBase(disp, "Menu Title") {
        mainMenu->addItem("Option", [this]() { /* action */ });
    }

    void setup() override {
        display->clearScreen();
        display->displayMainTitle("Your Page");
    }

    void loop() override {
        if (hasActiveMenu()) return;
        // page logic
    }

    void handleInput() override {
        handleBasicInputInteractions();
        // extra input logic
    }

    const char* getName() override { return "YourPage"; }
};
#endif
```

## Adding Menu Items

```cpp
// Simple action
mainMenu->addItem("Label", [this]() { /* ... */ });

// Submenu
void openSubMenu() {
    MenuHandler* sub = new MenuHandler(display, "Sub Title");
    sub->addItem("Item", [this]() { /* ... */ });
    menuManager->pushMenu(sub);   // back via B long press
}
```

## TimeSelector Usage

```cpp
// Seconds only
timeSelector->configureSeconds(15, 5, 120);  // default, min, max
timeSelector->setOnComplete([](TimeValue r) {
    SettingsManager::getInstance()->setAutoSleepDelay(r.seconds);
});
timeSelector->start();

// Hours + Minutes
timeSelector->configureHoursMinutes(14, 30);
timeSelector->setOnComplete([](TimeValue r) {
    rtcSetTime(r.hours, r.minutes, 0);
});
timeSelector->start();
```

When TimeSelector is active, route buttons:
- PWR → `timeSelector->navigateUp()`
- B short → `timeSelector->navigateDown()`
- A → `timeSelector->select()`

## Hardware Buttons Reference

```
      ______PWR   (side, near A)
                A (front, next to screen)
      ___B_____   (side, opposite, near screen)
```

Default PageBase routing (no menu active):
- **PWR**: nothing (override for page-change)
- **A**: open main menu
- **B short**: nothing (override for page action)
- **B long (800ms)**: close menu / go back
