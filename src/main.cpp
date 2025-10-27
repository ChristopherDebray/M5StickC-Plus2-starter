#include <M5StickCPlus2.h>
#include <Arduino.h>
#include <Preferences.h>

#include "../lib/button_handler.h"
#include "../lib/tap_handler.h"
#include "../lib/battery_handler.h"
#include "../lib/clock_handler.h"
#include "../lib/display_handler.h"
#include "../lib/rtc_utils.h"

// ---------- Settings ----------
#define POMO_MINUTES       1
#define CLOCK_REFRESH_MS   1000
// ------------------------------

Preferences prefs;

// Handlers
DisplayHandler displayHandler;
ClockHandler clockHandler(&displayHandler);
BatteryHandler batteryHandler(&displayHandler);
ButtonHandler btnB(1, 400);  // Button B, 400ms double-click
TapHandler tapDetector(1.5, 300, 3);  // 1.5G threshold, 300ms timeout, 3 taps

void beepAlarm() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(1.0f);
  for (int i = 0; i < 8; ++i) {
    M5.Speaker.tone(2500, 100);
    delay(250);
  }
  M5.Speaker.end();
}

// ========== CALLBACKS ==========

void onButtonDoubleClick() {
  clockHandler.armPomodoroAndSleep();
}

void onTripleTap(int tapCount) {
  M5.Lcd.fillScreen(BLUE);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(20, 60);
  M5.Lcd.printf("%d TAPS!", tapCount);
  M5.Speaker.tone(2500, 100);
  delay(1000);
  M5.Lcd.fillScreen(BLACK);
  clockHandler.drawClock(0);
}

// ========== SETUP ==========

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);

  // Initialize handlers
  btnB.onDoubleClick(onButtonDoubleClick);
  tapDetector.begin();
  tapDetector.onTaps(onTripleTap);
  batteryHandler.begin();

  // UNCOMMENT TO SET TIME
  // setRTCTime(14, 30, 0, 2025, 10, 27, 1);

  // End of Pomodoro?
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    clockHandler.clearTarget();
    beepAlarm();
  }

  M5.Lcd.fillScreen(BLACK);
  clockHandler.drawClock(0);
}

// ========== LOOP ==========

void loop() {
  M5.update();
  
  // Update handlers
  btnB.update();
  tapDetector.update();
  batteryHandler.update();
  batteryHandler.displayInfo();

  static unsigned long tRef = 0;
  if (millis() - tRef > 1000) {
    clockHandler.drawClock(0);
    tRef = millis();
  }

  delay(10);
}