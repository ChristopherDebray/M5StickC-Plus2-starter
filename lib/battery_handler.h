#ifndef BATTERY_HANDLER_H
#define BATTERY_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <driver/gpio.h>

#include "./display_handler.h"

class BatteryHandler {
private:
  int32_t bc;
  int32_t bl;
  int16_t bv;
  m5::Power_Class::is_charging_t ic;
  DisplayHandler* display;
  unsigned long lastUpdate;
  uint32_t updateInterval;
  
public:
  // Constructor
  BatteryHandler(DisplayHandler* displayHandler, uint32_t intervalMs = 5000) : display(displayHandler) {
    display = displayHandler;
    bc = 0;
    bl = 0;
    bv = 0;
    ic = m5::Power_Class::is_charging_t::is_discharging;
    lastUpdate = 0;
    updateInterval = intervalMs;
  }
  
  // Initialize (no need to call M5.begin again!)
  void begin() {
    // Just read initial values
    update();
  }

  // Update function - call this in loop()
  void update() {
    unsigned long now = millis();
    
    // Only update every X milliseconds to avoid constant screen refresh
    if (now - lastUpdate < updateInterval) {
      return;
    }
    lastUpdate = now;
    
    // Read battery data
    bc = M5.Power.getBatteryCurrent();
    bl = M5.Power.getBatteryLevel();
    bv = M5.Power.getBatteryVoltage();
    ic = M5.Power.isCharging();
  }

  static void M5deepSleep(uint64_t microseconds = 0) {
    // CRITICAL: Keep GPIO4 HIGH during deep sleep to maintain power
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    gpio_hold_en(GPIO_NUM_4);
    gpio_deep_sleep_hold_en();
    if (microseconds > 0) {
        esp_sleep_enable_timer_wakeup(microseconds);
    }
    
    esp_deep_sleep_start();
  }

  void cutAllNonCore() {
    esp_wifi_stop();
    btStop();
    M5.Speaker.end();
    M5.Lcd.sleep();
  }
  
  // Getters
  int32_t getCurrent() { return bc; }
  int32_t getLevel() { return bl; }
  int16_t getVoltage() { return bv; }
  bool isCharging() { return ic == m5::Power_Class::is_charging_t::is_charging; }
  
  // Display battery info on screen
  void displayInfo() {
    display->displayBatteryLevel(bl, getBatteryDisplayColor(bl), isCharging());
    
  }

  int getBatteryDisplayColor(int32_t batteryLevel) {
    if (batteryLevel > 80) {
        return GREEN;
    }
    if (batteryLevel > 60) {
        return BLUE;
    }

    if (batteryLevel > 30) {
        return YELLOW;
    }

    return RED;
  }
};

#endif