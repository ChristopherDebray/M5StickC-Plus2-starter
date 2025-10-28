#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <M5Unified.h>
#include <Arduino.h>
#include "./display_handler.h"
#include "./menu_handler.h"

#define MAX_MENU_STACK 5

class MenuManager {
private:
    DisplayHandler* display;
    MenuHandler* menuStack[MAX_MENU_STACK];
    int stackSize;
    bool isActive;
    
public:
    MenuManager(DisplayHandler* disp) : display(disp) {
        stackSize = 0;
        isActive = false;
    }
    
    ~MenuManager() {
        // Clean up all menus in stack
        while (stackSize > 0) {
            popMenu();
        }
    }
    
    // Push a new menu onto the stack (open menu)
    bool pushMenu(MenuHandler* menu) {
        if (stackSize >= MAX_MENU_STACK) {
            Serial.println("MenuManager: Stack full!");
            return false;
        }
        
        menuStack[stackSize] = menu;
        stackSize++;
        isActive = true;
        
        // Draw the new menu
        menu->draw();
        
        Serial.printf("MenuManager: Pushed menu (stack size: %d)\n", stackSize);
        return true;
    }
    
    // Pop current menu from stack (close menu / go back)
    bool popMenu() {
        if (stackSize <= 0) {
            Serial.println("MenuManager: Stack empty!");
            isActive = false;
            return false;
        }
        
        stackSize--;
        
        // If there's still a menu in stack, redraw it
        if (stackSize > 0) {
            menuStack[stackSize - 1]->draw();
        } else {
            isActive = false;
            // Clear screen when last menu is closed
            display->clearScreen();
        }
        
        Serial.printf("MenuManager: Popped menu (stack size: %d)\n", stackSize);
        return true;
    }
    
    // Get current (top) menu
    MenuHandler* getCurrentMenu() {
        if (stackSize > 0) {
            return menuStack[stackSize - 1];
        }
        return nullptr;
    }
    
    // Close all menus
    void closeAll() {
        while (stackSize > 0) {
            popMenu();
        }
        display->clearScreen();
        isActive = false;
    }
    
    // Navigation methods (forward to current menu)
    void navigateUp() {
        MenuHandler* current = getCurrentMenu();
        if (current) {
            current->moveUp();
            current->draw();
        }
    }
    
    void navigateDown() {
        MenuHandler* current = getCurrentMenu();
        if (current) {
            current->moveDown();
            current->draw();
        }
    }
    
    void select() {
        MenuHandler* current = getCurrentMenu();
        if (current) {
            current->select();
        }
    }
    
    // Getters
    bool hasActiveMenu() { return isActive && stackSize > 0; }
    int getStackSize() { return stackSize; }
};

#endif