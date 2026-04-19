#ifndef DISPLAY_HANDLER_M5STICK_ADAPTER_H
#define DISPLAY_HANDLER_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include "../ports/display_handler_port.h"

class DisplayHandlerM5StickAdapter : public IDisplayHandler {
public:
    DisplayHandlerM5StickAdapter() {}

    void clearScreen() override {
        M5.Display.fillScreen(BACKGROUND_COLOR);
    }

    void flashScreen(uint16_t color, int durationMs = 200) override {
        M5.Display.fillScreen(color);
        delay(durationMs);
        clearScreen();
    }

    void displayMainTitle(const char* text, MessageType type = MSG_NORMAL) override {
        M5.Display.setTextSize(SIZE_TITLE);
        M5.Display.setTextColor(colorFor(type));
        int x = centerX(text, SIZE_TITLE);
        M5.Display.setCursor(x, ZONE_CENTER_Y - 20);
        M5.Display.print(text);
    }

    void displaySubtitle(const char* text, MessageType type = MSG_NORMAL) override {
        M5.Display.setTextSize(SIZE_SUBTITLE);
        M5.Display.setTextColor(colorFor(type));
        int x = centerX(text, SIZE_SUBTITLE);
        M5.Display.setCursor(x, ZONE_CENTER_Y + 20);
        M5.Display.print(text);
    }

    void displayInfoMessage(const char* text, MessageType type = MSG_INFO) override {
        M5.Display.setTextSize(SIZE_BODY);
        M5.Display.setTextColor(colorFor(type));
        int x = centerX(text, SIZE_BODY);
        M5.Display.setCursor(x, MARGIN);
        M5.Display.print(text);
    }

    void displayStatus(const char* text, MessageType type = MSG_NORMAL) override {
        M5.Display.setTextSize(SIZE_BODY);
        M5.Display.setTextColor(colorFor(type));
        int x = centerX(text, SIZE_BODY);
        M5.Display.setCursor(x, ZONE_BOTTOM_Y);
        M5.Display.print(text);
    }

    void displayText(const char* text, DisplayZone zone, int textSize = 2, MessageType type = MSG_NORMAL) override {
        M5.Display.setTextSize(textSize);
        M5.Display.setTextColor(colorFor(type));
        int textWidth = strlen(text) * 6 * textSize;
        M5.Display.setCursor(xForZone(zone, textWidth), yForZone(zone));
        M5.Display.print(text);
    }

    void displayTextAt(const char* text, int x, int y, int textSize = 2, MessageType type = MSG_NORMAL) override {
        M5.Display.setTextSize(textSize);
        M5.Display.setTextColor(colorFor(type));
        M5.Display.setCursor(x, y);
        M5.Display.print(text);
    }

    void drawCenteredText(const char* text, int y, uint16_t color, int textSize = 2) override {
        M5.Display.setTextSize(textSize);
        M5.Display.setTextColor(color);
        M5.Display.setCursor(centerX(text, textSize), y);
        M5.Display.print(text);
    }

    void displayBatteryLevel(int level, int color, bool isCharging = false) override {
        M5.Display.setTextSize(SIZE_SMALL);
        M5.Display.setTextColor(color ? color : WHITE);
        M5.Display.setCursor(SCREEN_WIDTH - 40, 5);
        M5.Display.printf("%d%%", level);
        if (isCharging) {
            M5.Display.setCursor(SCREEN_WIDTH - 15, 5);
            M5.Display.print("+");
        }
    }

    void showLoading(const char* message = "Loading...") override {
        clearScreen();
        displayMainTitle(message);
    }

    void showFullScreenMessage(const char* title, const char* message,
                               MessageType type = MSG_INFO, int durationMs = 2000) override {
        clearScreen();
        displayMainTitle(title, type);
        if (message) displaySubtitle(message, type);
        delay(durationMs);
    }

    int getWidth() override  { return SCREEN_WIDTH; }
    int getHeight() override { return SCREEN_HEIGHT; }

private:
    static const int SCREEN_WIDTH  = 240;
    static const int SCREEN_HEIGHT = 135;
    static const int MARGIN        = 10;
    static const int ZONE_CENTER_Y = 60;
    static const int ZONE_BOTTOM_Y = 110;
    static const int SIZE_TITLE    = 4;
    static const int SIZE_SUBTITLE = 3;
    static const int SIZE_BODY     = 2;
    static const int SIZE_SMALL    = 1;

    static const uint16_t BACKGROUND_COLOR = BLACK;

    uint16_t colorFor(MessageType type) {
        switch(type) {
            case MSG_INFO:    return BLUE;
            case MSG_SUCCESS: return GREEN;
            case MSG_WARNING: return ORANGE;
            case MSG_ERROR:   return RED;
            case MSG_NORMAL:
            default:          return WHITE;
        }
    }

    int centerX(const char* text, int textSize) {
        return (SCREEN_WIDTH - (int)strlen(text) * 6 * textSize) / 2;
    }

    int xForZone(DisplayZone zone, int textWidth) {
        switch(zone) {
            case ZONE_TOP_LEFT:
            case ZONE_BOTTOM_LEFT:  return MARGIN;
            case ZONE_TOP_CENTER:
            case ZONE_CENTER:
            case ZONE_BOTTOM_CENTER: return (SCREEN_WIDTH - textWidth) / 2;
            case ZONE_TOP_RIGHT:
            case ZONE_BOTTOM_RIGHT: return SCREEN_WIDTH - textWidth - MARGIN;
            default:                return 0;
        }
    }

    int yForZone(DisplayZone zone) {
        switch(zone) {
            case ZONE_TOP_LEFT:
            case ZONE_TOP_CENTER:
            case ZONE_TOP_RIGHT:    return MARGIN;
            case ZONE_CENTER:       return ZONE_CENTER_Y;
            case ZONE_BOTTOM_LEFT:
            case ZONE_BOTTOM_CENTER:
            case ZONE_BOTTOM_RIGHT: return ZONE_BOTTOM_Y;
            default:                return 0;
        }
    }
};

#endif
