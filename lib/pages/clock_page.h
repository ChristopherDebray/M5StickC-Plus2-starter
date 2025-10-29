#ifndef CLOCK_PAGE_H
#define CLOCK_PAGE_H

#include "page_base.h"
#include "../clock_handler.h"
#include "../battery_handler.h"
#include "../settings_manager.h"

class ClockPage : public PageBase {
private:
    ClockHandler* clockHandler;
    BatteryHandler* batteryHandler;
    
    unsigned long lastClockUpdate;
    uint32_t clockRefreshInterval;
    
    MenuHandler* settingsMenu;
    
    // Buffers for dynamic labels
    char soundLabel[32];
    char timeFormatLabel[32];

    void updateMenuLabels() {
        sprintf(soundLabel, "UI Sound: %s", 
                settings->getUiSound() ? "ON" : "OFF");
        
        sprintf(timeFormatLabel, "Time: %s", 
                settings->getTime24h() ? "24h" : "12h");
    }

    void rebuildMainMenu() {
        mainMenu->clear();
        
        mainMenu->addItem("Start Pomodoro", [this]() { onStartPomodoro(); });
        mainMenu->addItem("Set Timer", [this]() { onSetTimer(); });
        mainMenu->addItem("Settings", [this]() { onSettings(); });
    }
    
    void rebuildSettingsMenu() {
        if (!settingsMenu) {
            settingsMenu = new MenuHandler(display, "Settings");
        } else {
            settingsMenu->clear();
        }
        
        updateMenuLabels();
        
        settingsMenu->addItem(soundLabel, [this]() { onToggleSound(); });
        settingsMenu->addItem(timeFormatLabel, [this]() { onToggleTimeFormat(); });
    }
    
    void onStartPomodoro() {
        clockHandler->armPomodoroAndSleep();
    }
    
    void onSetTimer() {
        display->showFullScreenMessage("Timer", "Coming soon", MSG_INFO, 1000);
        if (mainMenu) mainMenu->draw();
    }
    
    void onSettings() {
        rebuildSettingsMenu();
        menuManager->pushMenu(settingsMenu);
    }
    
    void onToggleSound() {
        bool current = settings->getUiSound();
        settings->setUiSound(!current);
        
        display->showFullScreenMessage(
            "UI Sound", 
            current ? "OFF" : "ON", 
            current ? MSG_ERROR : MSG_SUCCESS, 
            800
        );
        
        // Rebuild et redraw
        rebuildSettingsMenu();
        settingsMenu->draw();
    }
    
    void onToggleTimeFormat() {
        bool current = settings->getTime24h();
        settings->setTime24h(!current);
        
        display->showFullScreenMessage(
            "Time Format", 
            current ? "12h" : "24h", 
            MSG_INFO, 
            800
        );
        
        rebuildSettingsMenu();
        settingsMenu->draw();
    }
    
public:
    ClockPage(DisplayHandler* disp, ClockHandler* clock, BatteryHandler* battery)
        : PageBase(disp, "Clock Menu"),
          clockHandler(clock),
          batteryHandler(battery),
          settingsMenu(nullptr) {
        
        lastClockUpdate = 0;
        clockRefreshInterval = 1000;
        
        updateMenuLabels();
        rebuildMainMenu();
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