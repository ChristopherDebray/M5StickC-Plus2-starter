#ifndef CLOCK_HANDLER_H
#define CLOCK_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>
#include <Preferences.h>

#include "./display_handler.h"
#include "./rtc_utils.h" 

class ClockHandler {
private:
    m5::rtc_datetime_t dt;
    char timeBuffer[16];  // Buffer to store time, array of 16 caracters (string)
    char dateBuffer[16];  // Buffer to store date
    DisplayHandler* display;
public:
    Preferences prefs;
    static const int POMODORO_MINUTES = 25;
    static const int CLOCK_REFRESH_MS = 1000;

    // Constructor
    ClockHandler(DisplayHandler* displayHandler) : display(displayHandler) {
        memset(timeBuffer, 0, sizeof(timeBuffer));
        memset(dateBuffer, 0, sizeof(dateBuffer));
    }
    
    // Update internal datetime
    void updateDateTime() {
        M5.Rtc.getDateTime(&dt);
    }
    
    // Get current time as string (format: HH:MM:SS)
    const char* getCurrentFullTime() {
        updateDateTime();
        sprintf(timeBuffer, "%02d:%02d:%02d", dt.time.hours, dt.time.minutes, dt.time.seconds);
        return timeBuffer;
    }
    
    // Get current date in French format (format: DD-MM-YYYY)
    const char* getCurrentFullDateFR() {
        updateDateTime();
        sprintf(dateBuffer, "%02d-%02d-%04d", dt.date.date, dt.date.month, dt.date.year);
        return dateBuffer;
    }
    
    // Get current date in US format (format: MM-DD-YYYY)
    const char* getCurrentFullDateUS() {
        updateDateTime();
        sprintf(dateBuffer, "%02d-%02d-%04d", dt.date.month, dt.date.date, dt.date.year);
        return dateBuffer;
    }
    
    // Get current date in ISO format (format: YYYY-MM-DD)
    const char* getCurrentFullDateISO() {
        updateDateTime();
        sprintf(dateBuffer, "%04d-%02d-%02d", dt.date.year, dt.date.month, dt.date.date);
        return dateBuffer;
    }

    void drawClock(uint32_t remainSec = 0) {
        static uint32_t lastRemain = 9999;
        static uint8_t lastSec = 99;
        
        updateDateTime();
        
        if (dt.time.seconds != lastSec || remainSec != lastRemain) {
            display->clearScreen();
            
            // Display time using DisplayHandler
            display->displayMainTitle(getCurrentFullTime());
            
            // Display date
            display->displaySubtitle(getCurrentFullDateFR());
            
            // Display Pomodoro if active
            if (remainSec > 0) {
                char pomoText[32];
                sprintf(pomoText, "Pomo: %02u:%02u", remainSec / 60, remainSec % 60);
                display->displayInfoMessage(pomoText);
            }
            
            lastSec = dt.time.seconds;
            lastRemain = remainSec;
        }
    }

    void armPomodoroAndSleep() {
        display->clearScreen();
        display->displayMainTitle("Pomodoro", MSG_SUCCESS);
        // M5.Lcd.fillScreen(BLACK);
        // M5.Lcd.setTextSize(3);
        // M5.Lcd.setTextColor(GREEN);
        // M5.Lcd.setCursor(20, 40);
        // M5.Lcd.println("Pomodoro");
        // M5.Lcd.setCursor(30, 75);
        // M5.Lcd.printf("%u min", POMODORO_MINUTES);
        // M5.Lcd.setCursor(40, 110);
        // M5.Lcd.println("Zzz...");
        delay(1500);
        
        armTimerAndSleep(POMODORO_MINUTES);
    }

    void armTimerAndSleep(uint32_t minutes) {
        uint32_t now = rtcEpochNow();
        uint32_t target = now + minutes * 60;
        writeTarget(target);

        uint64_t us = (uint64_t)minutes * 60ULL * 1000000ULL;
        BatteryHandler::M5deepSleep(us);
    }

    void writeTarget(uint32_t epoch) {
        prefs.begin("clock", false);
        prefs.putUInt("target", epoch);
        prefs.end();
    }

    uint32_t readTarget() {
        prefs.begin("clock", true);
        uint32_t v = prefs.getUInt("target", 0);
        prefs.end();
        return v;
    }

    void clearTarget() { 
        writeTarget(0); 
    }
    
    // Get individual time components
    int getHours() { 
        updateDateTime();
        return dt.time.hours; 
    }
    
    int getMinutes() { 
        updateDateTime();
        return dt.time.minutes; 
    }
    
    int getSeconds() { 
        updateDateTime();
        return dt.time.seconds; 
    }
    
    // Get individual date components
    int getYear() { 
        updateDateTime();
        return dt.date.year; 
    }
    
    int getMonth() { 
        updateDateTime();
        return dt.date.month; 
    }
    
    int getDay() { 
        updateDateTime();
        return dt.date.date; 
    }
    
    int getWeekDay() { 
        updateDateTime();
        return dt.date.weekDay; 
    }
};

#endif