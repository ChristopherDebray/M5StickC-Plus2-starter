#ifndef TIME_SELECTOR_H
#define TIME_SELECTOR_H

#include <M5Unified.h>
#include <Arduino.h>
#include <functional>
#include "display_handler.h"
#include "settings_manager.h"

struct TimeValue {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    
    TimeValue() : hours(0), minutes(0), seconds(0) {}
    TimeValue(uint8_t h, uint8_t m, uint8_t s) : hours(h), minutes(m), seconds(s) {}
};

enum TimeField {
    FIELD_HOURS,
    FIELD_MINUTES,
    FIELD_SECONDS
};

struct TimeFieldConfig {
    TimeField field;
    const char* label;
    uint8_t minValue;
    uint8_t maxValue;
    uint8_t defaultValue;
};

class TimeSelector {
private:
    DisplayHandler* display;
    SettingsManager* settings;
    
    TimeFieldConfig* fields;
    uint8_t fieldCount;
    uint8_t currentFieldIndex;
    
    TimeValue currentValue;
    
    std::function<void(TimeValue)> onComplete;
    
    String title;
    bool active;
    
    static const int VALUE_Y = 50;
    static const int LABEL_Y = 90;
    
    // Variables for "virtual menu" (navigation like MenuHandler)
    int virtualMenuSize;
    int virtualCurrentIndex;
    
public:
    TimeSelector(DisplayHandler* disp, const char* titleText = "Set Time")
        : display(disp), currentFieldIndex(0), active(false), 
          virtualMenuSize(0), virtualCurrentIndex(0) {
        
        settings = SettingsManager::getInstance();
        title = String(titleText);
        fields = nullptr;
        fieldCount = 0;
    }
    
    ~TimeSelector() {
        if (fields) {
            delete[] fields;
        }
    }
    
    void configureSeconds(uint8_t defaultSec = 15, uint8_t minSec = 5, uint8_t maxSec = 60) {
        if (fields) delete[] fields;
        
        fieldCount = 1;
        fields = new TimeFieldConfig[1];
        fields[0] = {FIELD_SECONDS, "Seconds", minSec, maxSec, defaultSec};
        
        currentValue.seconds = defaultSec;
        currentFieldIndex = 0;
    }
    
    void configureHoursMinutes(uint8_t defaultHour = 0, uint8_t defaultMin = 0) {
        if (fields) delete[] fields;
        
        fieldCount = 2;
        fields = new TimeFieldConfig[2];
        fields[0] = {FIELD_HOURS, "Hours", 0, 23, defaultHour};
        fields[1] = {FIELD_MINUTES, "Minutes", 0, 59, defaultMin};
        
        currentValue.hours = defaultHour;
        currentValue.minutes = defaultMin;
        currentFieldIndex = 0;
    }
    
    void configureHoursMinutesSeconds(uint8_t h = 0, uint8_t m = 0, uint8_t s = 0) {
        if (fields) delete[] fields;
        
        fieldCount = 3;
        fields = new TimeFieldConfig[3];
        fields[0] = {FIELD_HOURS, "Hours", 0, 23, h};
        fields[1] = {FIELD_MINUTES, "Minutes", 0, 59, m};
        fields[2] = {FIELD_SECONDS, "Seconds", 0, 59, s};
        
        currentValue.hours = h;
        currentValue.minutes = m;
        currentValue.seconds = s;
        currentFieldIndex = 0;
    }
    
    void setOnComplete(std::function<void(TimeValue)> callback) {
        onComplete = callback;
    }
    
    void start() {
        active = true;
        currentFieldIndex = 0;
        
        // Creates a "virtual menu" with all possible values
        TimeFieldConfig& field = fields[currentFieldIndex];
        uint8_t* currentVal = getValuePointer(field.field);
        
        virtualMenuSize = field.maxValue - field.minValue + 1;
        virtualCurrentIndex = *currentVal - field.minValue;
        
        draw();
    }
    
    void stop() {
        active = false;
    }
    
    bool isActive() {
        return active;
    }
    
    void navigateUp() {
        if (!active || fieldCount == 0) return;
        
        TimeFieldConfig& field = fields[currentFieldIndex];
        uint8_t* currentVal = getValuePointer(field.field);
        
        virtualCurrentIndex--;
        if (virtualCurrentIndex < 0) {
            virtualCurrentIndex = virtualMenuSize - 1;
        }
        
        *currentVal = field.minValue + virtualCurrentIndex;
        
        if (settings->getUiSound()) {
            M5.Speaker.tone(2800, 30);
        }
        
        draw();
    }
    
    void navigateDown() {
        if (!active || fieldCount == 0) return;
        
        TimeFieldConfig& field = fields[currentFieldIndex];
        uint8_t* currentVal = getValuePointer(field.field);
        
        virtualCurrentIndex++;
        if (virtualCurrentIndex >= virtualMenuSize) {
            virtualCurrentIndex = 0;
        }
        
        *currentVal = field.minValue + virtualCurrentIndex;
        
        if (settings->getUiSound()) {
            M5.Speaker.tone(2400, 30);
        }
        
        draw();
    }
    
    void select() {
        if (!active || fieldCount == 0) return;
        
        if (settings->getUiSound()) {
            M5.Speaker.tone(3000, 50);
        }
        
        currentFieldIndex++;
        
        if (currentFieldIndex >= fieldCount) {
            // Selection fully completed
            active = false;
            if (onComplete) {
                onComplete(currentValue);
            }
        } else {
            // Go to next step
            TimeFieldConfig& field = fields[currentFieldIndex];
            uint8_t* currentVal = getValuePointer(field.field);
            
            virtualMenuSize = field.maxValue - field.minValue + 1;
            virtualCurrentIndex = *currentVal - field.minValue;
            
            draw();
        }
    }
    
    void draw() {
        if (!active || fieldCount == 0) return;
        
        display->clearScreen();
        
        TimeFieldConfig& field = fields[currentFieldIndex];
        uint8_t* currentVal = getValuePointer(field.field);
        
        display->drawCenteredText(title.c_str(), 10, TFT_WHITE, 2);
        
        // Progression (setps on top right of screen)
        char progress[16];
        sprintf(progress, "%d/%d", currentFieldIndex + 1, fieldCount);
        M5.Display.setTextColor(TFT_DARKGREY);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(M5.Display.width() - 30, 10);
        M5.Display.print(progress);
        
        uint8_t prevVal = (*currentVal == field.minValue) ? field.maxValue : (*currentVal - 1);
        char prevStr[8];
        sprintf(prevStr, "%02d", prevVal);
        display->drawCenteredText(prevStr, VALUE_Y - 20, TFT_DARKGREY, 2);
        
        char valueStr[8];
        sprintf(valueStr, "%02d", *currentVal);
        display->drawCenteredText(valueStr, VALUE_Y + 10, TFT_YELLOW, 4);
        
        uint8_t nextVal = (*currentVal == field.maxValue) ? field.minValue : (*currentVal + 1);
        char nextStr[8];
        sprintf(nextStr, "%02d", nextVal);
        display->drawCenteredText(nextStr, VALUE_Y + 50, TFT_DARKGREY, 2);
        
        display->drawCenteredText(field.label, LABEL_Y, TFT_CYAN, 1);
        
        // Instructions
        M5.Display.setTextColor(TFT_DARKGREY);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(5, M5.Display.height() - 15);
        M5.Display.print("PWR/B:Nav A:OK");
    }
    
private:
    uint8_t* getValuePointer(TimeField field) {
        switch(field) {
            case FIELD_HOURS:
                return &currentValue.hours;
            case FIELD_MINUTES:
                return &currentValue.minutes;
            case FIELD_SECONDS:
                return &currentValue.seconds;
            default:
                return &currentValue.seconds;
        }
    }
};

#endif