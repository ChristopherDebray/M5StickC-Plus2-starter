#ifndef PAGE_BASE_H
#define PAGE_BASE_H

#include <M5Unified.h>
#include <Arduino.h>
#include "../display_handler.h"
#include "../menu_manager.h"
#include "../menu_handler.h"
#include "../settings_manager.h"

class PageBase {
protected:
    bool initialized;
    DisplayHandler* display;
    MenuManager* menuManager;
    MenuHandler* mainMenu;
    
    // Button B long press tracking
    unsigned long btnBPressStart;
    const unsigned long LONG_PRESS_DURATION = 800;
    bool btnBLongPressTriggered;
    
    // Virtual methods for custom button behavior
    
    // Default: navigate menu down
    virtual void onButtonPWRPressed() {
        if (hasActiveMenu()) {
            navigateMenuUp();
        }
    }
    
    // Default: select or open menu
    virtual void onButtonAPressed() {
        if (hasActiveMenu()) {
            selectMenuItem();
        } else {
            openMenu();
        }
    }
    
    // Default: navigate menu up
    virtual void onButtonBShortPress() {
        if (hasActiveMenu()) {
            navigateMenuDown();
        }
    }
    
    // Default: close menu
    virtual void onButtonBLongPress() {
        if (hasActiveMenu()) {
            closeMenu();
        }
    }
    
public:
    PageBase(DisplayHandler* disp, const char* menuTitle = "Options") 
        : initialized(false), display(disp) {
        menuManager = new MenuManager(disp);
        mainMenu = new MenuHandler(disp, menuTitle);
        btnBPressStart = 0;
        btnBLongPressTriggered = false;
    }
    
    virtual ~PageBase() {
        delete mainMenu;
        delete menuManager;
    }
    
    virtual void setup() = 0;
    virtual void loop() = 0;
    
    virtual void cleanup() {
        if (menuManager) {
            menuManager->closeAll();
        }
    }
    
    virtual const char* getName() = 0;
    
    bool isInitialized() { return initialized; }
    void setInitialized(bool value) { initialized = value; }
    
    // Input handling - called by PageManager
    void handleInput() {
        // Button PWR
        if (M5.BtnPWR.wasPressed()) {
            onButtonPWRPressed();
        }
        
        // Button A
        if (M5.BtnA.wasPressed()) {
            onButtonAPressed();
        }
        
        // Button B with long press detection
        if (M5.BtnB.wasPressed()) {
            btnBPressStart = millis();
            btnBLongPressTriggered = false;
        }
        
        if (M5.BtnB.isPressed()) {
            unsigned long pressDuration = millis() - btnBPressStart;
            
            if (pressDuration >= LONG_PRESS_DURATION && !btnBLongPressTriggered) {
                onButtonBLongPress();
                btnBLongPressTriggered = true;
            }
        }
        
        if (M5.BtnB.wasReleased()) {
            if (!btnBLongPressTriggered) {
                onButtonBShortPress();
            }
        }
    }
    
    // Menu management
    bool hasActiveMenu() {
        return menuManager && menuManager->hasActiveMenu();
    }
    
    void openMenu() {
        if (!hasActiveMenu() && mainMenu) {
            menuManager->pushMenu(mainMenu);
            if (SettingsManager::getInstance()->getUiSound()) {
                M5.Speaker.tone(2500, 50);
            }
        }
    }
    
    void closeMenu() {
        if (menuManager && menuManager->popMenu()) {
            if (SettingsManager::getInstance()->getUiSound()) {
                M5.Speaker.tone(2000, 50);
            }
            
            if (!menuManager->hasActiveMenu()) {
                setup();
            }
        }
    }
    
    void navigateMenuUp() {
        if (menuManager) {
            menuManager->navigateUp();
        }
    }
    
    void navigateMenuDown() {
        if (menuManager) {
            menuManager->navigateDown();
        }
    }
    
    void selectMenuItem() {
        if (menuManager) {
            menuManager->select();
        }
    }
    
    MenuManager* getMenuManager() { return menuManager; }
    MenuHandler* getMainMenu() { return mainMenu; }
    DisplayHandler* getDisplay() { return display; }
};

#endif