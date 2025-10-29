#include <M5Unified.h>
#include <Arduino.h>
#include <Preferences.h>

#include "../lib/settings_manager.h"
#include "../lib/display_handler.h"
#include "../lib/battery_handler.h"
#include "../lib/rtc_utils.h"
#include "../lib/clock_handler.h"
#include "../lib/page_manager.h"
#include "../lib/pages/clock_page.h"

DisplayHandler displayHandler;
ClockHandler clockHandler(&displayHandler);
BatteryHandler batteryHandler(&displayHandler);

PageManager pageManager;
SettingsManager* settings;
ClockPage* clockPage = nullptr;

void beepAlarm() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(1.0f);
  for (int i = 0; i < 8; ++i) {
    M5.Speaker.tone(2500, 200);
    delay(250);
  }
  M5.Speaker.end();
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(3);
  Serial.begin(115200);
  
  settings = SettingsManager::getInstance();
  settings->begin();
  batteryHandler.begin();

  clockPage = new ClockPage(&displayHandler, &clockHandler, &batteryHandler);
  pageManager.addPage(clockPage);

  // @todo add a tag system using pref maybe to allow for different actions based of tags
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    clockHandler.clearTarget();
    beepAlarm();
  }

  pageManager.begin();
}

void loop() {
  M5.update();
  
  // PageManager handles everything: input routing and page updates
  pageManager.handleInput();
  pageManager.update();
  
  // Update global handlers
  batteryHandler.update();
  if (settings->shouldGoToSleep()) {
    batteryHandler.M5deepSleep();
  }
  delay(10);
}