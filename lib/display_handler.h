#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <M5Unified.h>
#include <Arduino.h>

// Display zones and positions
enum DisplayZone {
    ZONE_TOP_LEFT,
    ZONE_TOP_CENTER,
    ZONE_TOP_RIGHT,
    ZONE_CENTER,
    ZONE_BOTTOM_LEFT,
    ZONE_BOTTOM_CENTER,
    ZONE_BOTTOM_RIGHT,
    ZONE_CUSTOM
};

// Message types with associated colors
enum MessageType {
    MSG_INFO,
    MSG_SUCCESS,
    MSG_WARNING,
    MSG_ERROR,
    MSG_NORMAL
};

class DisplayHandler {
private:
    // Screen dimensions (M5StickCPlus2 in rotation 3)
    const int SCREEN_WIDTH = 240;
    const int SCREEN_HEIGHT = 135;

    const int MARGIN = 10;
    
    // Predefined zones (Y positions)
    const int ZONE_TOP_Y = MARGIN;
    const int ZONE_CENTER_Y = 60;
    const int ZONE_BOTTOM_Y = 110;
    
    // Text sizes
    const int SIZE_TITLE = 4;
    const int SIZE_SUBTITLE = 3;
    const int SIZE_BODY = 2;
    const int SIZE_SMALL = 1;

    const uint16_t BACKGROUND_COLOR = BLACK;
    
    // Colors by message type
    uint16_t getColorForType(MessageType type) {
        switch(type) {
            case MSG_INFO:    return BLUE;
            case MSG_SUCCESS: return GREEN;
            case MSG_WARNING: return ORANGE;
            case MSG_ERROR:   return RED;
            case MSG_NORMAL:
            default:          return WHITE;
        }
    }
    
    // Calculate X position based on zone and text width
    int getXForZone(DisplayZone zone, int textWidth) {
        switch(zone) {
            case ZONE_TOP_LEFT:
            case ZONE_BOTTOM_LEFT:
                return MARGIN;
            
            case ZONE_TOP_CENTER:
            case ZONE_CENTER:
            case ZONE_BOTTOM_CENTER:
                return (SCREEN_WIDTH - textWidth) / 2;
            
            case ZONE_TOP_RIGHT:
            case ZONE_BOTTOM_RIGHT:
                return SCREEN_WIDTH - textWidth - MARGIN;
            
            case ZONE_CUSTOM:
            default:
                return 0;
        }
    }
    
    // Calculate Y position based on zone
    int getYForZone(DisplayZone zone) {
        switch(zone) {
            case ZONE_TOP_LEFT:
            case ZONE_TOP_CENTER:
            case ZONE_TOP_RIGHT:
                return ZONE_TOP_Y;
            
            case ZONE_CENTER:
                return ZONE_CENTER_Y;
            
            case ZONE_BOTTOM_LEFT:
            case ZONE_BOTTOM_CENTER:
            case ZONE_BOTTOM_RIGHT:
                return ZONE_BOTTOM_Y;
            
            case ZONE_CUSTOM:
            default:
                return 0;
        }
    }

public:
    // Constructor
    DisplayHandler() {}
    
    // Clear entire screen
    void clearScreen() {
        M5.Display.fillScreen(BACKGROUND_COLOR);
    }
    
    // Flash screen with color
    void flashScreen(uint16_t color, int durationMs = 200) {
        M5.Display.fillScreen(color);
        delay(durationMs);
        clearScreen();
    }
    
    // Display main title (big, centered)
    void displayMainTitle(const char* text, MessageType type = MSG_NORMAL) {
        M5.Display.setTextSize(SIZE_TITLE);
        M5.Display.setTextColor(getColorForType(type));
        
        int textWidth = strlen(text) * 6 * SIZE_TITLE; // Approximate
        int x = getXForZone(ZONE_CENTER, textWidth);
        int y = getYForZone(ZONE_CENTER) - 20;
        
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }
    
    // Display subtitle (below main title)
    void displaySubtitle(const char* text, MessageType type = MSG_NORMAL) {
        M5.Display.setTextSize(SIZE_SUBTITLE);
        M5.Display.setTextColor(getColorForType(type));
        
        int textWidth = strlen(text) * 6 * SIZE_SUBTITLE;
        int x = getXForZone(ZONE_CENTER, textWidth);
        int y = getYForZone(ZONE_CENTER) + 20;
        
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }
    
    // Display info message at top
    void displayInfoMessage(const char* text, MessageType type = MSG_INFO) {
        M5.Display.setTextSize(SIZE_BODY);
        M5.Display.setTextColor(getColorForType(type));
        
        int textWidth = strlen(text) * 6 * SIZE_BODY;
        int x = getXForZone(ZONE_TOP_CENTER, textWidth);
        int y = getYForZone(ZONE_TOP_CENTER);
        
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }
    
    // Display status at bottom
    void displayStatus(const char* text, MessageType type = MSG_NORMAL) {
        M5.Display.setTextSize(SIZE_BODY);
        M5.Display.setTextColor(getColorForType(type));
        
        int textWidth = strlen(text) * 6 * SIZE_BODY;
        int x = getXForZone(ZONE_BOTTOM_CENTER, textWidth);
        int y = getYForZone(ZONE_BOTTOM_CENTER);
        
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }
    
    // Display text at specific zone
    void displayText(const char* text, DisplayZone zone, 
                    int textSize = 2, MessageType type = MSG_NORMAL) {
        M5.Display.setTextSize(textSize);
        M5.Display.setTextColor(getColorForType(type));
        
        int textWidth = strlen(text) * 6 * textSize;
        int x = getXForZone(zone, textWidth);
        int y = getYForZone(zone);
        
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }
    
    // Display text at custom position
    void displayTextAt(const char* text, int x, int y, 
                      int textSize = 2, MessageType type = MSG_NORMAL) {
        M5.Display.setTextSize(textSize);
        M5.Display.setTextColor(getColorForType(type));
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }
    
    // Display formatted text (like printf)
    void displayFormatted(DisplayZone zone, int textSize, MessageType type, 
                         const char* format, ...) {
        char buffer[128];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        displayText(buffer, zone, textSize, type);
    }
    
    // Display battery indicator (small, top right)
    void displayBatteryLevel(int level, int color, bool isCharging = false) {
        M5.Display.setTextSize(SIZE_SMALL);
        M5.Display.setTextColor(color ? color : WHITE);
        M5.Display.setCursor(SCREEN_WIDTH - 40, 5);
        M5.Display.printf("%d%%", level);
        
        if (isCharging) {
            M5.Display.setCursor(SCREEN_WIDTH - 15, 5);
            M5.Display.print("+");
        }
    }
    
    // Show loading animation
    void showLoading(const char* message = "Loading...") {
        clearScreen();
        displayMainTitle(message);
        // Could add spinner animation here
    }
    
    // Show full screen message
    void showFullScreenMessage(const char* title, const char* message, 
                              MessageType type = MSG_INFO, int durationMs = 2000) {
        clearScreen();
        displayMainTitle(title, type);
        if (message) {
            displaySubtitle(message, type);
        }
        delay(durationMs);
    }
    
    // Get screen dimensions
    int getWidth() { return SCREEN_WIDTH; }
    int getHeight() { return SCREEN_HEIGHT; }
};

#endif