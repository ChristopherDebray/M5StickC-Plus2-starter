# M5StickC-Plus2 Starter

> ⚠️ This is a work in progress and most of the interaction for now is in the main.cpp ⚠️

A comprehensive starter kit for M5StickC-Plus2 development, providing a solid foundation with essential features so you can focus on building your unique project.

## 🎯 Philosophy

Unlike traditional libraries that abstract away the implementation, this starter kit gives you **full access to the code**. You can understand, modify, and extend every part of the system. It's not a black box—it's a foundation you can build upon.

The goal is to eliminate the "boring but necessary" setup work, so you can dive straight into the creative parts of your project.

## 📋 Requirements

- **M5StickC-Plus2** device
- **VSCode** with **PlatformIO** extension
- M5Unified library (automatically installed via PlatformIO)

⚠️ **Note**: This project is specifically designed for the **M5StickC-Plus2** and is not compatible with the older M5StickC-Plus or other M5Stack devices.

⚠️ **Arduino IDE**: While this project was initially started with Arduino IDE, all development has since moved to PlatformIO. Arduino IDE compatibility is not guaranteed.

## 🚀 Quick Start

1. Clone or download this repository
2. Open the project in VSCode with PlatformIO extension
3. Connect your M5StickC-Plus2
4. Click "Upload" in PlatformIO
5. You're ready to go!

## 📖 Terminology

### Hardware Buttons

For clarity and consistency with the M5Unified library documentation:
```
      ______PWR               (Side button, near button A)
                    A         (Front button, next to screen)
      ___B_____               (Side button, opposite side, near screen)
```

- **A button**: The button on the front, next to the screen
- **PWR button**: The side button near the A button (also used for power)
- **B button**: The side button on the opposite side

*Refer to the image for visual clarification*

### File Naming Conventions

- **`*_manager.h`**: Complex orchestrators handling high-level operations (e.g., page navigation, menu stacks)
- **`*_handler.h`**: Focused, reusable components for specific tasks (think: mini-libraries)
- **`*_utils.h`**: Collections of related utility functions that can be used across multiple contexts (e.g., time conversion, RTC operations)

## ✨ Features

### 🎨 Core Systems

#### Page System
- ✅ Display management based on active page
- ✅ Multiple page support
- ✅ Page navigation (previous/next)
- ✅ Each page has its own `setup()`, `loop()`, and `cleanup()` lifecycle

#### Menu System
- ✅ Stack-based menu management (LIFO - Last In, First Out)
- ✅ Menu items with labels and callbacks
- ✅ Nested submenus support
- ✅ Navigation (up/down)
- ✅ Back/close functionality
- ⬜ Configurable navigation sounds
- ⬜ Custom tone selection

#### Wifi helper

> **⚠️ This is a modified version from another personnal project, i didn't had time to test this version for now ⚠️**

- ✅ WiFi connection with timeout after 10s
- ✅ getIp
- ✅ getSignalStrenght


### Optional systems

#### MQTT helper

> **⚠️ This is a modified version from another personnal project, i didn't had time to test this version for now ⚠️**

- MQTT Helper - Simplified MQTT client wrapper

**Features:**

    - Auto-reconnect on disconnect

    - JSON payload support
    - Callback system for messages
    - QoS support

**Dependencies:**

- AsyncMqttClient library

- ArduinoJson library

> Add to platformio.ini:
```
  lib_deps = 
    marvinroger/AsyncMqttClient@^0.8.1
    bblanchon/ArduinoJson@^7.4.2
```

### 🧩 Handlers

#### `clock_handler.h`
Manages all time-related functionality:
- ✅ Get current time (hours, minutes, seconds)
- ✅ Get current date (multiple formats: FR, US, ISO)
- ✅ Display time and date on screen
- ✅ Pomodoro timer (25-minute focus sessions)
- ⬜ Customizable timers with user-defined durations

#### `display_handler.h`
Abstracts display positioning with semantic zones:
- ✅ **Display zones**: Place text without "magic numbers"
  - `ZONE_TOP_LEFT`, `ZONE_CENTER`, `ZONE_BOTTOM_RIGHT`, etc.
- ✅ **Message types**: Color-coded by purpose
  - `MSG_INFO` (blue), `MSG_SUCCESS` (green), `MSG_WARNING` (orange), `MSG_ERROR` (red)
- ✅ **Pre-built layouts**:
  - Main title (large, centered)
  - Subtitle (below title)
  - Info messages (small, top)
  - Status messages (bottom)
  - Battery indicator (top-right corner)
- ✅ Screen flash effects
- ✅ Full-screen messages with auto-dismiss

#### `battery_handler.h`
Power management and battery monitoring:
- ✅ Get battery level, voltage, and current
- ✅ Charging status detection
- ✅ Color-coded battery display (red < 30%, yellow < 60%, blue < 80%, green > 80%)
- ✅ **`M5deepSleep()`**: Proper deep sleep implementation (critical for battery life!)
- ✅ **`cutAllNonCore()`**: Disable WiFi, Bluetooth, and speaker for power saving
- ⬜ Configurable auto-sleep on idle
- ⬜ Wake on button press

#### `time_selector.h`
Interactive time/value selection interface:

✅ **Three configuration modes:**

- Seconds only (5-60s range, ideal for timers)

- Hours + Minutes (0-23h, 0-59m, for time setting)
- Hours + Minutes + Seconds (full time configuration)

- ✅ Menu-style navigation


✅ **Visual feedback:**

- Current value (large, center)

- Preview of previous/next values
- Field label (Hours/Minutes/Seconds)
- Progress indicator (1/2, 2/2, etc.)


- ✅ Value wrapping: Automatically loops (59→0, 0→59)

- ✅ Callback system: Execute custom code when selection is complete

Example Usage:
```cpp
// Configure for seconds selection (auto-sleep delay)
timeSelector->configureSeconds(15, 5, 120);  // Default: 15s, Range: 5-120s

timeSelector->setOnComplete([](TimeValue result) {
    settings->setAutoSleepDelay(result.seconds);
});

timeSelector->start();
```

Example Usage (Time Setting):
```cpp
// Configure for time setting
timeSelector->configureHoursMinutes(14, 30);  // Default: 14:30

timeSelector->setOnComplete([](TimeValue result) {
    rtcSetTime(result.hours, result.minutes, 0);
});

timeSelector->start();
```

#### `settings_manager.h`

Persistent settings with Preferences (EEPROM):

✅ **UI Settings:**
- Sound effects (on/off)


✅ **Power Management:**

- Auto-sleep (on/off)

- Auto-sleep delay (5-120 seconds, configurable)
- Inactivity timer with automatic reset


✅ **Data Persistence:** All settings saved to flash memory

- ✅ **Singleton Pattern:** Single instance accessible everywhere

- ✅ **Easy Integration:** Simple getter/setter methods

Example Usage:
```cpp
SettingsManager* settings = SettingsManager::getInstance();

// Check if auto-sleep should trigger
if (settings->shouldGoToSleep()) {
    batteryHandler.M5deepSleep();
}

// Reset inactivity timer on user interaction
settings->resetInactivityTimer();
```

#### `menu_handler.h`
Individual menu creation and interaction:
- ✅ Add menu items with callbacks
- ✅ Navigate through items (up/down with scroll)
- ✅ Select items to trigger actions
- ✅ Enable/disable items
- ✅ Visual feedback (selection highlight, scroll indicators)
- ✅ Audio feedback on navigation and selection

#### `rtc_utils.h`
Real-Time Clock utilities:
- ✅ Convert system time to RTC format
- ✅ Get current epoch time
- ✅ Set RTC time and date

### 🎨 UI Components

#### TimeSelector Widget

The TimeSelector provides a reusable interface for selecting time values with an intuitive, menu-like experience.
Key Features:

- Flexible Configuration: Seconds-only, Hours+Minutes, or full Hours+Minutes+Seconds

- Smart Navigation: PWR/B buttons for up/down, with automatic value wrapping
- Visual Clarity: Large centered value with preview of adjacent values
- Progress Tracking: Shows current field (e.g., "1/2" when setting hours)
- Callback-Driven: Execute custom logic when selection is complete

**Common Use Cases:**

- Auto-Sleep Configuration: Select delay in seconds (5-60s)

- Manual Time Setting: Set hours and minutes separately
- Timer Configuration: Set countdown duration
- Any Numeric Input: Easily adaptable for other value ranges

Integration with Pages:

```cpp
class YourPage : public PageBase {
private:
    TimeSelector* timeSelector;
    
    void onButtonPWRPressed() override {
        if (timeSelector->isActive()) {
            timeSelector->navigateUp();
        } else if (hasActiveMenu()) {
            navigateMenuUp();
        }
    }
    
    void onButtonBShortPress() override {
        if (timeSelector->isActive()) {
            timeSelector->navigateDown();
        } else if (hasActiveMenu()) {
            navigateMenuDown();
        }
    }
    
    void onButtonAPressed() override {
        if (timeSelector->isActive()) {
            timeSelector->select();
        } else if (hasActiveMenu()) {
            selectMenuItem();
        }
    }
};
```

### 🎛️ Managers

#### `menu_manager.h`
Stack-based menu orchestration:
- ✅ Push menus onto stack (open menu/submenu)
- ✅ Pop menus from stack (go back)
- ✅ Close all menus at once
- ✅ Automatic menu redrawing on stack changes

Example flow:
```
Clock Page → Open Menu → Settings → Display Settings → [Back] → Settings → [Back] → Clock Page
```

#### `page_manager.h`
Page lifecycle management:
- ✅ Register multiple pages
- ✅ Switch between pages
- ✅ Automatic `setup()` and `cleanup()` calls on page transitions

### 📄 Default Pages

#### Clock Page (`clock_page.h`)
- ✅ Real-time clock display (HH:MM:SS)
- ✅ Date display (DD-MM-YYYY)
- ✅ Battery level indicator
- ✅ Options menu:
  - ⬜ Start Pomodoro timer
  - ⬜ Set custom timer
  - ⬜ Configure time/date
    - ✅ Configure time
    - ⬜ Configure date
  - ✅ Settings submenu
    - ✅ UI Sound toggle (on/off)
    
    - ✅ Auto-sleep toggle (on/off)
    - ✅ Auto-sleep delay configuration (5-120 seconds)

#### Menu Page (`menu_page.h`)
- ✅ Example page demonstrating menu system
- ✅ Sample menu items with callbacks

## 🎮 Default Controls

### Without Menu Active
- **PWR button**: Change page (previous)
- **A button**: Open menu (if available on current page)
- **B button**: (Page-specific action, e.g., start Pomodoro on Clock page)

### With Menu Active
- **PWR button (press)**: Navigate down ↓
- **A button (press)**: Select item
- **B button (press)**: Navigate up ↑
- **B button (hold ~800ms)**: Close menu / Go back

## 🏗️ Project Structure
```
M5StickC-Plus2-starter/
├── src/
│   └── main.cpp                    # Application entry point
├── lib/
│   ├── display_handler.h           # Display utilities
│   ├── clock_handler.h             # Time and timers
│   ├── battery_handler.h           # Power management
│   ├── menu_handler.h              # Individual menu logic
│   ├── menu_manager.h              # Menu stack orchestration
│   ├── page_manager.h              # Page lifecycle
│   ├── button_handler.h            # Button input detection
│   ├── tap_handler.h               # Accelerometer tap detection
│   ├── rtc_utils.h                 # RTC utilities
│   ├── power_utils.h               # Power functions
│   ├── pages/
│   │   ├── page_base.h             # Base class for all pages
│   │   ├── clock_page.h            # Default clock page
│   │   └── menu_page.h             # Example menu page
└── platformio.ini                  # PlatformIO configuration
```

## 🔧 Extending the Starter Kit

### Creating a New Page

1. Create a new file in `lib/pages/your_page.h`
2. Inherit from `PageBase`
3. Implement required methods
```cpp
#ifndef YOUR_PAGE_H
#define YOUR_PAGE_H

#include "page_base.h"

class YourPage : public PageBase {
public:
    YourPage(DisplayHandler* disp) 
        : PageBase(disp, "Your Menu Title") {
        
        // Add menu items
        mainMenu->addItem("Option 1", [this]() { 
            // Your action here
        });
    }
    
    void setup() override {
        display->clearScreen();
        display->displayMainTitle("Your Page");
    }
    
    void loop() override {
        if (hasActiveMenu()) return;
        // Your page logic here
    }
    
    const char* getName() override { 
        return "YourPage"; 
    }
};

#endif
```

4. Register your page in `main.cpp`:
```cpp
#include "../lib/pages/your_page.h"

YourPage yourPage(&displayHandler);
pageManager.addPage(&yourPage);
```

### Adding Menu Items with Callbacks
```cpp
mainMenu->addItem("My Action", [this]() {
    display->showFullScreenMessage("Action", "Executed!", MSG_SUCCESS, 1000);⚠️ Known Issues & Fixes
});
```

### Creating Submenus
```cpp
void onOpenSettings() {
    MenuHandler* settingsMenu = new MenuHandler(display, "Settings");
    
    settingsMenu->addItem("Display", [this]() { 
        /* display settings */ 
    });
    
    settingsMenu->addItem("Sound", [this]() { 
        /* sound settings */ 
    });
    
    // Push submenu onto stack
    menuManager->pushMenu(settingsMenu);
}
```

## ⚠️ Known Issues & Fixes

### Button Detection Issues (PWR and B buttons)

**Problem:** On M5StickC Plus 2, M5.BtnPWR.wasPressed() and M5.BtnB.wasPressed() may not trigger reliably in some contexts.

**Root Cause:** These buttons can get "stuck" in a pressed state internally, preventing wasPressed() from detecting new presses.

**Solution:** I implemented manual edge detection in `PageBase::handleBasicInputInteractions()`:

```cpp
// Instead of relying on wasPressed()
bool currentPWRState = M5.BtnPWR.isPressed();
if (currentPWRState && !lastBtnPWRState) {  // Rising edge
    onButtonPWRPressed();
}
lastBtnPWRState = currentPWRState;
```

This approach detects state changes (rising/falling edges) manually, providing reliable button input across all contexts (menus, TimeSelector, etc.).

**Impact:** All button inputs now work consistently, including:

- Menu navigation
- TimeSelector value adjustment
- Long press detection (B button)

### Deep Sleep GPIO Hold

**Problem:** M5StickC Plus 2 loses power immediately after entering deep sleep.

**Root Cause:** GPIO4 must remain HIGH during deep sleep to maintain power rail.

**Solution:** Implemented in battery_handler.h:

```cpp
pinMode(4, OUTPUT);
digitalWrite(4, HIGH);
gpio_hold_en(GPIO_NUM_4);         // Hold GPIO state
gpio_deep_sleep_hold_en();        // Maintain during deep sleep
```

**Impact:** Device now properly enters deep sleep while maintaining power, enabling:

- Timer wakeup
- Low-power standby mode
- Extended battery life

## 🤝 Contributing

This is a starter kit, not a library. Feel free to:
- Fork and modify for your needs
- Share your improvements
- Use it as a base for your own projects

## 📝 License

Copyright 2025 Christopher Debray

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## 🙏 Acknowledgments

- M5Stack for the M5Unified library
- The PlatformIO team for their excellent development platform

---

**Happy Making! 🎉**