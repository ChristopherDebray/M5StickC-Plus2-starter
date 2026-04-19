#ifndef DISPLAY_HANDLER_PORT_H
#define DISPLAY_HANDLER_PORT_H

#include <stdint.h>

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

enum MessageType {
    MSG_INFO,
    MSG_SUCCESS,
    MSG_WARNING,
    MSG_ERROR,
    MSG_NORMAL
};

class IDisplayHandler {
public:
    virtual ~IDisplayHandler() = default;

    virtual void clearScreen() = 0;
    virtual void flashScreen(uint16_t color, int durationMs = 200) = 0;
    virtual void displayMainTitle(const char* text, MessageType type = MSG_NORMAL) = 0;
    virtual void displaySubtitle(const char* text, MessageType type = MSG_NORMAL) = 0;
    virtual void displayInfoMessage(const char* text, MessageType type = MSG_INFO) = 0;
    virtual void displayStatus(const char* text, MessageType type = MSG_NORMAL) = 0;
    virtual void displayText(const char* text, DisplayZone zone, int textSize = 2, MessageType type = MSG_NORMAL) = 0;
    virtual void displayTextAt(const char* text, int x, int y, int textSize = 2, MessageType type = MSG_NORMAL) = 0;
    virtual void drawCenteredText(const char* text, int y, uint16_t color, int textSize = 2) = 0;
    virtual void displayBatteryLevel(int level, int color, bool isCharging = false) = 0;
    virtual void showLoading(const char* message = "Loading...") = 0;
    virtual void showFullScreenMessage(const char* title, const char* message, MessageType type = MSG_INFO, int durationMs = 2000) = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
};

#endif
