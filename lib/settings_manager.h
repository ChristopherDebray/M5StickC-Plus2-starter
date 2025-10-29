#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Preferences.h>
#include <Arduino.h>

// Singleton with cache for settings since prefs reading is slow and power consuming
class SettingsManager {
private:
    static SettingsManager* instance;
    Preferences prefs;
    
    // Settings cache read only ONCE on startup
    struct {
        bool uiSound;
        uint8_t brightness;
        bool time24h;
        uint8_t pomodoroMinutes;
        uint8_t shortBreakMinutes;
        uint8_t longBreakMinutes;
        bool autoSleep;
        uint16_t autoSleepDelay;
    } cache;
    
    SettingsManager() {
        // Private constructor (singleton)
    }
    
public:
    // Get singleton instance
    static SettingsManager* getInstance() {
        if (instance == nullptr) {
            instance = new SettingsManager();
        }
        return instance;
    }
    
    // Load settings from NVS into cache (called ONCE at setup setup)
    void begin() {
        prefs.begin("settings", true);
        
        // Load UI settings
        cache.uiSound = prefs.getBool("ui_sound", true);
        cache.brightness = prefs.getUChar("brightness", 128);
        
        // Load time settings (not used yet)
        cache.time24h = prefs.getBool("time_24h", true);
        
        // Load power settings (not used yet)
        cache.autoSleep = prefs.getBool("auto_sleep", false);
        cache.autoSleepDelay = prefs.getUShort("sleep_delay", 30);
        
        prefs.end();
    }
    
    // GETTERS (just read the cache)
    
    bool getUiSound() { return cache.uiSound; }
    uint8_t getBrightness() { return cache.brightness; }
    bool getTime24h() { return cache.time24h; }
    uint8_t getPomodoroMinutes() { return cache.pomodoroMinutes; }
    uint8_t getShortBreakMinutes() { return cache.shortBreakMinutes; }
    uint8_t getLongBreakMinutes() { return cache.longBreakMinutes; }
    bool getAutoSleep() { return cache.autoSleep; }
    uint16_t getAutoSleepDelay() { return cache.autoSleepDelay; }
    
    // SETTERS (update cache + save to NVS)
    
    void setUiSound(bool value) {
        cache.uiSound = value;
        prefs.begin("settings", false);
        prefs.putBool("ui_sound", value);
        prefs.end();
        Serial.printf("UI Sound: %s\n", value ? "ON" : "OFF");
    }
    
    void setTime24h(bool value) {
        cache.time24h = value;
        prefs.begin("settings", false);
        prefs.putBool("time_24h", value);
        prefs.end();
    }
    
    void setAutoSleep(bool value) {
        cache.autoSleep = value;
        prefs.begin("settings", false);
        prefs.putBool("auto_sleep", value);
        prefs.end();
    }
    
    void setAutoSleepDelay(uint16_t seconds) {
        cache.autoSleepDelay = seconds;
        prefs.begin("settings", false);
        prefs.putUShort("sleep_delay", seconds);
        prefs.end();
    }
    
    // Reset to defaults
    void resetToDefaults() {
        prefs.begin("settings", false);
        prefs.clear();
        prefs.end();
        
        // Reload defaults
        begin();
    }
    
    // Debug: print all settings
    void printAll() {
        Serial.println("=== Current Settings ===");
        Serial.printf("UI Sound: %s\n", cache.uiSound ? "ON" : "OFF");
        Serial.printf("Time Format: %s\n", cache.time24h ? "24h" : "12h");
        Serial.printf("Auto Sleep: %s\n", cache.autoSleep ? "ON" : "OFF");
        Serial.printf("Sleep Delay: %d sec\n", cache.autoSleepDelay);
        Serial.println("========================");
    }
};

// Initialize static instance
SettingsManager* SettingsManager::instance = nullptr;

#endif