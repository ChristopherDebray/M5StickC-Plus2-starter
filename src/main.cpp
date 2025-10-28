#include <M5Unified.h>
#include <Arduino.h>
#include <Preferences.h>

#include "../lib/display_handler.h"
#include "../lib/battery_handler.h"
#include "../lib/rtc_utils.h"
#include "../lib/clock_handler.h"
#include "../lib/page_manager.h"
#include "../lib/pages/clock_page.h"

Preferences prefs;

DisplayHandler displayHandler;
ClockHandler clockHandler(&displayHandler);
BatteryHandler batteryHandler(&displayHandler);

PageManager pageManager;
ClockPage clockPage(&displayHandler, &clockHandler, &batteryHandler);

// Button B long press tracking
unsigned long btnBPressStart = 0;
const unsigned long LONG_PRESS_DURATION = 800;
bool btnBLongPressTriggered = false;

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

  batteryHandler.begin();

  pageManager.addPage(&clockPage);

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    clockHandler.clearTarget();
    beepAlarm();
  }

  pageManager.begin();
  Serial.println("System ready!");
}

void loop() {
  M5.update();
  
  PageBase* currentPage = pageManager.getCurrentPage();
  bool menuActive = currentPage->hasActiveMenu();
  
  if (M5.BtnPWR.wasPressed()) {
    if (menuActive) {
      currentPage->navigateMenuUp();
    } else {
      pageManager.previousPage();
    }
  }
  
  if (M5.BtnA.wasPressed()) {
    if (menuActive) {
      currentPage->selectMenuItem();
    } else {
      if (pageManager.getCurrentPageIndex() == 0) {
        ClockPage* cp = static_cast<ClockPage*>(currentPage);
        cp->openMenu();
      }
    }
  }
  
  if (M5.BtnB.wasPressed()) {
    btnBPressStart = millis();
    btnBLongPressTriggered = false;
  }
  
  if (M5.BtnB.isPressed()) {
    unsigned long pressDuration = millis() - btnBPressStart;
    
    if (pressDuration >= LONG_PRESS_DURATION && !btnBLongPressTriggered) {
      // LONG PRESS = CLOSE MENU / BACK
      if (menuActive) {
        currentPage->closeMenu();
      }
      btnBLongPressTriggered = true;
    }
  }
  
  if (M5.BtnB.wasReleased()) {
    if (!btnBLongPressTriggered && menuActive) {
      // SHORT PRESS = Navigate UP
      currentPage->navigateMenuDown();
    }
  }
  
  // Update
  batteryHandler.update();
  pageManager.update();

  delay(10);
}