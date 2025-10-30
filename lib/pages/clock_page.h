#ifndef CLOCK_PAGE_H
#define CLOCK_PAGE_H

#include "page_base.h"
#include "../clock_handler.h"
#include "../battery_handler.h"
#include "../settings_manager.h"
#include "../time_selector.h"
#include "../rtc_utils.h"

class ClockPage : public PageBase {
private:
    ClockHandler* clockHandler;
    BatteryHandler* batteryHandler;
    
    unsigned long lastClockUpdate;
    uint32_t clockRefreshInterval;
    
    MenuHandler* settingsMenu;
    TimeSelector* timeSelector;
    
    char soundLabel[32];
    char timeFormatLabel[32];
    char autoSleepLabel[32];
    char sleepDelayLabel[48];

    void updateMenuLabels() {
        sprintf(soundLabel, "UI Sound: %s", 
                settings->getUiSound() ? "ON" : "OFF");
        
        sprintf(timeFormatLabel, "Time: %s", 
                settings->getTime24h() ? "24h" : "12h");
        
        sprintf(autoSleepLabel, "Auto Sleep: %s",
                settings->getAutoSleep() ? "ON" : "OFF");
        
        sprintf(sleepDelayLabel, "Sleep Delay: %ds",
                settings->getAutoSleepDelay());
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
        settingsMenu->addItem(autoSleepLabel, [this]() { onToggleAutoSleep(); });
        settingsMenu->addItem(sleepDelayLabel, [this]() { onConfigureSleepDelay(); });
        settingsMenu->addItem("Set Time", [this]() { onSetTime(); });
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
    
    void onToggleAutoSleep() {
        bool current = settings->getAutoSleep();
        settings->setAutoSleep(!current);
        
        display->showFullScreenMessage(
            "Auto Sleep", 
            current ? "OFF" : "ON", 
            current ? MSG_ERROR : MSG_SUCCESS, 
            800
        );
        
        rebuildSettingsMenu();
        settingsMenu->draw();
    }
    
    void onConfigureSleepDelay() {
        menuManager->closeAll();
        
        uint16_t currentDelay = settings->getAutoSleepDelay();
        timeSelector->configureSeconds(currentDelay, 5, 120);
        
        timeSelector->setOnComplete([this](TimeValue result) {
            settings->setAutoSleepDelay(result.seconds);
            
            char msg[32];
            sprintf(msg, "%d seconds", result.seconds);
            display->showFullScreenMessage("Sleep Delay", msg, MSG_SUCCESS, 1000);
            
            rebuildSettingsMenu();
            menuManager->pushMenu(settingsMenu);
        });
        
        timeSelector->start();
    }
    
    void onSetTime() {
        menuManager->closeAll();
        
        uint8_t currentHour = rtcGetHours();
        uint8_t currentMinute = rtcGetMinutes();
        
        timeSelector->configureHoursMinutes(currentHour, currentMinute);
        
        timeSelector->setOnComplete([this](TimeValue result) {
            // Udpate RTC with new time, keep seconds to 0 just to simplify
            rtcSetTime(result.hours, result.minutes, 0);
            
            char msg[32];
            sprintf(msg, "%02d:%02d", result.hours, result.minutes);
            display->showFullScreenMessage("Time Set", msg, MSG_SUCCESS, 1000);
            
            rebuildSettingsMenu();
            menuManager->pushMenu(settingsMenu);
        });
        
        timeSelector->start();
    }
    
    // Callback overrides to handle TimeSelector
    void onButtonPWRPressed() override {
        if (timeSelector->isActive()) {
            timeSelector->navigateUp();
        } else if (hasActiveMenu()) {
            navigateMenuUp();
        }
    }
    
    void onButtonAPressed() override {
        if (timeSelector->isActive()) {
            timeSelector->select();
        } else if (hasActiveMenu()) {
            selectMenuItem();
        } else {
            openMenu();
        }
    }
    
    void onButtonBShortPress() override {
        if (timeSelector->isActive()) {
            timeSelector->navigateDown();
        } else if (hasActiveMenu()) {
            navigateMenuDown();
        }
    }
    
public:
    ClockPage(DisplayHandler* disp, ClockHandler* clock, BatteryHandler* battery)
        : PageBase(disp, "Clock Menu"),
          clockHandler(clock),
          batteryHandler(battery),
          settingsMenu(nullptr) {
        
        settings = SettingsManager::getInstance();
        
        lastClockUpdate = 0;
        clockRefreshInterval = 1000;
        
        timeSelector = new TimeSelector(display, "Set Time");
        
        updateMenuLabels();
        rebuildMainMenu();
    }
    
    ~ClockPage() {
        if (settingsMenu) {
            delete settingsMenu;
        }
        if (timeSelector) {
            delete timeSelector;
        }
    }
    
    void setup() override {
        display->clearScreen();
        clockHandler->drawClock(0);
        lastClockUpdate = millis();
    }
    
    void loop() override {
        if (timeSelector->isActive()) {
            return;
        }
        
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
    
    void handleInput() override {
        handleBasicInputInteractions();
    }
    
    const char* getName() override {
        return "Clock";
    }
    
    ClockHandler* getClockHandler() { return clockHandler; }
};

#endif