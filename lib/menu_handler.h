#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>
#include "./display_handler.h"

#define MAX_MENU_ITEMS 20

struct MenuItem {
    const char* label;
    std::function<void()> callback;
    bool enabled;
    
    MenuItem() : label(""), callback(nullptr), enabled(true) {}
    MenuItem(const char* lbl, std::function<void()> cb) : label(lbl), callback(cb), enabled(true) {}
};

class MenuHandler {
private:
    DisplayHandler* display;
    MenuItem items[MAX_MENU_ITEMS];
    int itemCount;
    int selectedIndex;
    int scrollOffset;
    int maxVisibleItems;
    
    const char* title;
    
    // Visual settings
    const int ITEM_HEIGHT = 20;
    const int START_Y = 30;
    const int TEXT_SIZE = 2;

public:
    MenuHandler(DisplayHandler* disp, const char* menuTitle = "Menu") 
        : display(disp), title(menuTitle) {
        itemCount = 0;
        selectedIndex = 0;
        scrollOffset = 0;
        maxVisibleItems = 4;  // Show 4 items at a time on M5Stick screen
    }
    
    // Add a menu item to the pull / list
    bool addItem(const char* label, std::function<void()> callback) {
        if (itemCount >= MAX_MENU_ITEMS) {
            return false;
        }
        items[itemCount] = MenuItem(label, callback);
        itemCount++;
        return true;
    }
    
    // Clear all items
    void clear() {
        itemCount = 0;
        selectedIndex = 0;
        scrollOffset = 0;
    }
    
    void moveUp() {
        if (selectedIndex > 0) {
            selectedIndex--;
            
            // Scroll up if needed
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            }
            
            // M5.Speaker.tone(2000, 30);
        }
    }
    
    void moveDown() {
        if (selectedIndex < itemCount - 1) {
            selectedIndex++;
            
            // Scroll down if needed
            if (selectedIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = selectedIndex - maxVisibleItems + 1;
            }
            
            // M5.Speaker.tone(2000, 30);
        }
    }
    
    // Select current item and launch it's associated callback
    void select() {
        if (selectedIndex >= 0 && selectedIndex < itemCount) {
            MenuItem& item = items[selectedIndex];
            
            if (item.enabled && item.callback != nullptr) {
                M5.Speaker.tone(2500, 50);
                delay(50);
                M5.Speaker.tone(3000, 50);
                
                item.callback();
            }
        }
    }
    
    // Draw the menu
    void draw() {
        display->clearScreen();
        
        display->displayTextAt(title, 10, 5, 1, MSG_INFO);
        
        // Draw scroll indicator (top)
        if (scrollOffset > 0) {
            display->displayTextAt("^", 220, 5, 1, MSG_NORMAL);
        }
        
        for (int i = 0; i < maxVisibleItems && (scrollOffset + i) < itemCount; i++) {
            int itemIndex = scrollOffset + i;
            MenuItem& item = items[itemIndex];
            
            int y = START_Y + (i * ITEM_HEIGHT);
            
            // Draw selection indicator + highlight
            if (itemIndex == selectedIndex) {
                M5.Lcd.fillRect(0, y - 2, 240, ITEM_HEIGHT, DARKGREY);
                display->displayTextAt(">", 5, y, TEXT_SIZE, MSG_SUCCESS);
            }
            
            MessageType color = item.enabled ? MSG_NORMAL : MSG_ERROR;
            display->displayTextAt(item.label, 25, y, TEXT_SIZE, color);
        }
        
        // Draw scroll indicator (bottom)
        if (scrollOffset + maxVisibleItems < itemCount) {
            display->displayTextAt("v", 220, 120, 1, MSG_NORMAL);
        }
        
        // Draw counter ( 1/10 )
        char counter[16];
        sprintf(counter, "%d/%d", selectedIndex + 1, itemCount);
        display->displayTextAt(counter, 180, 5, 1, MSG_NORMAL);
    }
    
    // Enable/disable an item
    void setItemEnabled(int index, bool enabled) {
        if (index >= 0 && index < itemCount) {
            items[index].enabled = enabled;
        }
    }
    
    // Getters
    int getSelectedIndex() { return selectedIndex; }
    int getItemCount() { return itemCount; }
    const char* getSelectedLabel() {
        if (selectedIndex >= 0 && selectedIndex < itemCount) {
            return items[selectedIndex].label;
        }
        return "";
    }
    
    // Reset selection to top
    void resetSelection() {
        selectedIndex = 0;
        scrollOffset = 0;
    }
};

#endif