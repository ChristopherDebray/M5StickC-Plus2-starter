#ifndef CLOCK_PAGE_H
#define CLOCK_PAGE_H

#include "page_base.h"
#include "../clock_handler.h"
#include "../battery_handler.h"

class ClockPage : public PageBase {
private:
    ClockHandler* clockHandler;
    BatteryHandler* batteryHandler;
    
    unsigned long lastClockUpdate;
    uint32_t clockRefreshInterval;
    
    // Sub-menus
    MenuHandler* settingsMenu;
    
    // Menu callbacks (private methods)
    void onStartPomodoro() {
        Serial.println("ClockMenu: Start Pomodoro");
        clockHandler->armPomodoroAndSleep();
    }
    
    void onSetTimer() {
        Serial.println("ClockMenu: Set Timer");
        // TODO: Timer configuration
    }
    
    void onSettings() {
        Serial.println("ClockMenu: Settings");
        
        // Create settings submenu if not exists
        if (!settingsMenu) {
            settingsMenu = new MenuHandler(display, "Settings");
            
            // Use lambdas to capture 'this' and call member functions
            settingsMenu->addItem("Display", [this]() { onSettingsDisplay(); });
            settingsMenu->addItem("Sound", [this]() { onSettingsSound(); });
            settingsMenu->addItem("Time Format", [this]() { onSettingsTimeFormat(); });
            settingsMenu->addItem("Back", [this]() { 
                // Close current menu
                menuManager->popMenu();
            });
        }
        
        // Push submenu onto stack
        menuManager->pushMenu(settingsMenu);
    }
    
    void onSettingsDisplay() {
        Serial.println("Settings: Display");
        display->showFullScreenMessage("Display", "Brightness, rotation...", MSG_INFO, 1000);
    }
    
    void onSettingsSound() {
        Serial.println("Settings: Sound");
        display->showFullScreenMessage("Sound", "Volume, tone...", MSG_INFO, 1000);
    }
    
    void onSettingsTimeFormat() {
        Serial.println("Settings: Time Format");
        display->showFullScreenMessage("Time Format", "24h / 12h", MSG_INFO, 1000);
    }
    
    void onStatistics() {
        Serial.println("ClockMenu: Statistics");
        display->showFullScreenMessage("Statistics", "Coming soon...", MSG_INFO, 1000);
    }
    
    void onAbout() {
        Serial.println("ClockMenu: About");
        display->showFullScreenMessage("M5 Clock", "v1.0.0", MSG_INFO, 1500);
    }
    
public:
    ClockPage(DisplayHandler* disp, ClockHandler* clock, BatteryHandler* battery)
        : PageBase(disp, "Clock Menu"),
          clockHandler(clock),
          batteryHandler(battery),
          settingsMenu(nullptr) {
        
        lastClockUpdate = 0;
        clockRefreshInterval = 1000;
        
        // Use lambdas with 'this' capture to call member functions
        mainMenu->addItem("Start Pomodoro", [this]() { onStartPomodoro(); });
        mainMenu->addItem("Set Timer", [this]() { onSetTimer(); });
        mainMenu->addItem("Settings", [this]() { onSettings(); });
        mainMenu->addItem("Statistics", [this]() { onStatistics(); });
        mainMenu->addItem("About", [this]() { onAbout(); });
    }
    
    ~ClockPage() {
        if (settingsMenu) {
            delete settingsMenu;
        }
    }
    
    void setup() override {
        display->clearScreen();
        clockHandler->drawClock(0);
        lastClockUpdate = millis();
    }
    
    void loop() override {
        if (hasActiveMenu()) {
            return;
        }
        
        unsigned long now = millis();
        if (now - lastClockUpdate >= clockRefreshInterval) {
            uint32_t target = clockHandler->readTarget();
            uint32_t remain = 0;
            
            if (target) {
                uint32_t nowE = rtcEpochNow();
                if (nowE < target) {
                    remain = target - nowE;
                } else {
                    clockHandler->clearTarget();
                }
            }
            
            clockHandler->drawClock(remain);
            batteryHandler->displayInfo();
            lastClockUpdate = now;
        }
    }
    
    const char* getName() override {
        return "Clock";
    }
    
    ClockHandler* getClockHandler() { return clockHandler; }
};

#endif