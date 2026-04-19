#include <M5Unified.h>
#include <Arduino.h>
#include <Preferences.h>

#include "../lib/settings_manager.h"
#include "../lib/dependancies/display_handler_deps.h"
#include "../lib/dependancies/battery_handler_deps.h"
#include "../lib/dependancies/rtc_utils_deps.h"
#include "../lib/dependancies/clock_handler_deps.h"
#include "../lib/dependancies/page_manager_deps.h"
#include "../lib/pages/clock_page.h"

IDisplayHandler* displayHandler = getM5StickDisplayHandler();
IBatteryHandler* batteryHandler = getM5StickBatteryHandler(displayHandler);
IRtcUtils*       rtcUtils       = getM5StickRtcUtils();
IClockHandler*   clockHandler   = getM5StickClockHandler(displayHandler, batteryHandler, rtcUtils);
IPageManager*    pageManager    = getM5StickPageManager();

SettingsManager* settings;
ClockPage*       clockPage = nullptr;

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
  batteryHandler->begin();

  clockPage = new ClockPage(displayHandler, clockHandler, batteryHandler, rtcUtils);
  pageManager->addPage(clockPage);

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    clockHandler->clearTarget();
    beepAlarm();
  }

  pageManager->begin();
}

void loop() {
  M5.update();

  pageManager->handleInput();
  pageManager->update();

  batteryHandler->update();
  if (settings->shouldGoToSleep()) {
    batteryHandler->deepSleep();
  }
  delay(10);
}
