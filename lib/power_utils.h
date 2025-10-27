#ifndef POWER_UTILS_H
#define POWER_UTILS_H

#include <M5StickCPlus2.h>
#include <M5Unified.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <driver/gpio.h>

// Turn off radios / sound / screen (power saving)
void cutAllNonCore() {
  esp_wifi_stop();
  btStop();
  M5.Speaker.end();
  M5.Lcd.sleep();
}

// Deep sleep with GPIO4 hold for M5StickCPlus2
void M5deepSleep(uint64_t microseconds) {
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

void batteryIndicatorSetup()
{
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setTextSize(2);
}

void displayBatteryLevel()
{
  int32_t bc = M5.Power.getBatteryCurrent();
  int32_t bl = M5.Power.getBatteryLevel();
  int16_t bv = M5.Power.getBatteryVoltage();
  m5::Power_Class::is_charging_t ic = M5.Power.isCharging();

  M5.Display.setCursor(0, 0);
  M5.Display.printf("Battery Monitor v 1.1\n\n");
  M5.Display.printf("Charge Current: %03d mA\n", bc);
  M5.Display.printf("Level         : %03d %%\n", bl);
  M5.Display.printf("Voltage       : %04d mV\n", bv);
  M5.Display.printf("Is Charging   : %01d\n", ic);

  delay(1000);
}

#endif