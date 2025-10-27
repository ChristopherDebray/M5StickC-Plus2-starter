#ifndef POWER_UTILS_H
#define POWER_UTILS_H

#include <M5StickCPlus2.h>
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

#endif