#ifndef BATTERY_HANDLER_H
#define BATTERY_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>

class BatteryHandler {
private:
  int32_t bc;
  int32_t bl;
  int16_t bv;
  m5::Power_Class::is_charging_t ic;
  
  unsigned long lastUpdate;
  uint32_t updateInterval;
  
public:
  // Constructor
  BatteryHandler(uint32_t intervalMs = 5000) {
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
  
  // Getters
  int32_t getCurrent() { return bc; }
  int32_t getLevel() { return bl; }
  int16_t getVoltage() { return bv; }
  bool isCharging() { return ic == m5::Power_Class::is_charging_t::is_charging; }
  
  // Display battery info on screen
  void displayInfo() {
    // M5.Display.setCursor(200, 0);
    // M5.Display.setTextSize(2);
    // M5.Display.printf("Battery: %d%%\n", bl);
    // M5.Display.printf("Voltage: %dmV\n", bv);
    // M5.Display.printf("%s\n", isCharging() ? "Charging" : "Battery");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(210, 5);
    M5.Lcd.setTextColor(getBatteryDisplayColor(bl));
    M5.Lcd.printf("%d%%", bl);
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