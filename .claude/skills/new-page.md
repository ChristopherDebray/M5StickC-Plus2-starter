# Skill: new-page

Scaffold a new page for the M5StickC-Plus2 firmware.

## Trigger

User says: "create a new page", "add a page", "scaffold page", "/new-page [PageName]"

## Instructions

Ask the user for:
1. Page class name (e.g. `StopwatchPage`)
2. Menu title string shown in the UI
3. Menu items needed (names + actions)

Then:
1. Create `lib/pages/<snake_case_name>.h` using the template below
2. Add `#include` + instantiation + `pageManager.addPage(...)` in `src/main.cpp`
3. Do NOT add error handling, logging, or features not requested

## Template

```cpp
#ifndef <UPPER_SNAKE>_H
#define <UPPER_SNAKE>_H

#include "page_base.h"

class <ClassName> : public PageBase {
public:
    <ClassName>(DisplayHandler* disp) : PageBase(disp, "<Menu Title>") {
        mainMenu->addItem("<Item>", [this]() {
            // action
        });
    }

    void setup() override {
        display->clearScreen();
        display->displayMainTitle("<Page Title>");
    }

    void loop() override {
        if (hasActiveMenu()) return;
        // page update logic (called every ~10ms)
    }

    void handleInput() override {
        handleBasicInputInteractions();
        // additional input logic if needed
    }

    const char* getName() override { return "<ClassName>"; }

    // Override only what differs from PageBase defaults:
    // void onButtonPWRPressed() override { ... }
    // void onButtonBShortPress() override { ... }
    // void onButtonAPressed() override { ... }
    // void onButtonBLongPress() override { ... }
};

#endif
```

## Critical Reminders
- Call `handleBasicInputInteractions()` in `handleInput()` — never skip it
- Use `hasActiveMenu()` guard at top of `loop()`
- Button PWR and B use manual edge detection inside PageBase — do NOT call `wasPressed()` on them
- Button A: `wasPressed()` is fine
- Submenus: `new MenuHandler(display, "Title")` then `menuManager->pushMenu(sub)` — never `delete` it manually (MenuManager owns it)
