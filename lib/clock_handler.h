#ifndef CLOCK_HANDLER_H
#define CLOCK_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>
#include <Preferences.h>

class ClockHandler {
private:
    m5::rtc_datetime_t dt;
    char timeBuffer[16];  // Buffer to store time, array of 16 caracters (string)
    char dateBuffer[16];  // Buffer to store date
    
public:
    Preferences prefs;

    // Constructor
    ClockHandler() {
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
    
    // Draw clock on screen
    void drawClock(uint32_t remainSec = 0) {
        static uint32_t lastRemain = 9999;
        static uint8_t lastSec = 99;
        
        updateDateTime();
        
        // Only redraw if seconds changed or pomodoro status changed
        if (dt.time.seconds != lastSec || remainSec != lastRemain) {
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setTextColor(WHITE);
            
            // Display time
            M5.Lcd.setTextSize(4);
            M5.Lcd.setCursor(5, 30);
            M5.Lcd.print(getCurrentFullTime());
            
            // Display date
            M5.Lcd.setTextSize(3);
            M5.Lcd.setCursor(10, 80);
            M5.Lcd.print(getCurrentFullDateFR());
            
            // Display Pomodoro if active
            if (remainSec > 0) {
                M5.Lcd.setTextSize(2);
                M5.Lcd.setTextColor(GREEN);
                M5.Lcd.setCursor(10, 120);
                M5.Lcd.printf("Pomo: %02u:%02u", remainSec / 60, remainSec % 60);
            }
            
            lastSec = dt.time.seconds;
            lastRemain = remainSec;
        }
    }

    void writeTarget(uint32_t epoch) {
        prefs.begin("clock", false);
        prefs.putUInt("target", epoch);
        prefs.end();
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