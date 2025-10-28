#ifndef PAGE_BASE_H
#define PAGE_BASE_H

#include <M5Unified.h>
#include <Arduino.h>
#include "../display_handler.h"
#include "../menu_manager.h"
#include "../menu_handler.h"

// Base class for all pages
class PageBase {
protected:
    bool initialized;
    DisplayHandler* display;
    MenuManager* menuManager;
    MenuHandler* mainMenu;
    
public:
    PageBase(DisplayHandler* disp, const char* menuTitle = "Options") 
        : initialized(false), display(disp) {
        // Create menu manager for this page
        menuManager = new MenuManager(disp);
        
        // Create default main menu (can be customized by child classes)
        mainMenu = new MenuHandler(disp, menuTitle);
    }
    
    virtual ~PageBase() {
        delete mainMenu;
        delete menuManager;
    }
    
    // Called once when entering the page
    virtual void setup() = 0;
    
    // Called every loop iteration
    virtual void loop() = 0;
    
    // Called once when leaving the page
    virtual void cleanup() {
        if (menuManager) {
            menuManager->closeAll();
        }
    }
    
    // Page identification
    virtual const char* getName() = 0;
    
    // Initialization
    bool isInitialized() { return initialized; }
    void setInitialized(bool value) { initialized = value; }
    
    // Menu management (standardized for all pages)
    bool hasActiveMenu() {
        return menuManager && menuManager->hasActiveMenu();
    }
    
    void openMenu() {
        if (!hasActiveMenu() && mainMenu) {
            menuManager->pushMenu(mainMenu);
            M5.Speaker.tone(2500, 50);
        }
    }
    
    void closeMenu() {
        if (menuManager && menuManager->popMenu()) {
            M5.Speaker.tone(2000, 50);
            
            // If no more menus, redraw page
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
    
    // Access to menu manager for advanced usage
    MenuManager* getMenuManager() { return menuManager; }
    MenuHandler* getMainMenu() { return mainMenu; }
    
    // Access to display handler
    DisplayHandler* getDisplay() { return display; }
};

#endif