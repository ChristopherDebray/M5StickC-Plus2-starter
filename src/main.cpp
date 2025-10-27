#include <M5StickCPlus2.h>
#include <Arduino.h>
#include <Preferences.h>

#include "../lib/power_utils.h"
#include "../lib/button_handler.h"
#include "../lib/tap_handler.h"
#include "../lib/rtc_utils.h"

// ---------- Settings ----------
#define POMO_MINUTES       25
#define CLOCK_REFRESH_MS   1000
// ------------------------------

Preferences prefs;
RTC_DATA_ATTR uint8_t bootCounter = 0;

// Handlers
ButtonHandler btnB(1, 400);  // Button B, 400ms double-click
TapHandler tapDetector(1.5, 300, 3);  // 1.5G threshold, 300ms timeout, 3 taps

// Pomodoro target (epoch sec) persisted
uint32_t readTarget() {
  prefs.begin("clock", true);
  uint32_t v = prefs.getUInt("target", 0);
  prefs.end();
  return v;
}

void writeTarget(uint32_t epoch) {
  prefs.begin("clock", false);
  prefs.putUInt("target", epoch);
  prefs.end();
}

void clearTarget() { 
  writeTarget(0); 
}

void drawClock(uint32_t remainSec = 0) {
  static uint32_t lastRemain = 9999;
  static uint8_t lastSec = 99;
  
  m5::rtc_datetime_t dt;
  M5.Rtc.getDateTime(&dt);
  
  if (dt.time.seconds != lastSec || remainSec != lastRemain) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    
    M5.Lcd.setTextSize(4);
    char hhmmss[16];
    sprintf(hhmmss, "%02d:%02d:%02d", dt.time.hours, dt.time.minutes, dt.time.seconds);
    M5.Lcd.setCursor(5, 30);
    M5.Lcd.print(hhmmss);
    
    M5.Lcd.setTextSize(3);
    char ymd[16];
    sprintf(ymd, "%02d-%02d-%04d", dt.date.date, dt.date.month, dt.date.year);
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.print(ymd);
    
    if (remainSec > 0) {
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10, 120);
      M5.Lcd.printf("Pomo: %02u:%02u", remainSec/60, remainSec%60);
    }
    
    lastSec = dt.time.seconds;
    lastRemain = remainSec;
  }
}

void beepAlarm() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(1.0f);
  for (int i = 0; i < 8; ++i) {
    M5.Speaker.tone(2500, 200);
    delay(250);
  }
  M5.Speaker.end();
}

void showClock10sThenSleep() {
  M5.Lcd.wakeup();
  M5.Lcd.setRotation(3);
  drawClock(0);

  unsigned long t0 = millis();
  while (millis() - t0 < 10000) {
    M5.update();
    static unsigned long tr = 0;
    if (millis() - tr > CLOCK_REFRESH_MS) {
      drawClock(0);
      tr = millis();
    }
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
      return;
    }
    delay(10);
  }

  cutAllNonCore();
  esp_deep_sleep_start();
}

void armAndSleep(uint32_t minutes) {
  uint32_t now = rtcEpochNow();
  uint32_t target = now + minutes * 60;
  writeTarget(target);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setCursor(20, 40);
  M5.Lcd.println("Pomodoro");
  M5.Lcd.setCursor(30, 75);
  M5.Lcd.printf("%u min", minutes);
  M5.Lcd.setCursor(40, 110);
  M5.Lcd.println("Zzz...");
  delay(1500);

  cutAllNonCore();
  uint64_t us = (uint64_t)minutes * 60ULL * 1000000ULL;
  M5deepSleep(us);
}

// ========== CALLBACKS ==========

void onButtonDoubleClick() {
  armAndSleep(POMO_MINUTES);
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
  drawClock(0);
}

// ========== SETUP ==========

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  bootCounter++;

  // Initialize handlers
  btnB.onDoubleClick(onButtonDoubleClick);
  tapDetector.begin();
  tapDetector.onTaps(onTripleTap);

  // UNCOMMENT TO SET TIME
  // setRTCTime(14, 30, 0, 2025, 10, 27, 1);

  // End of Pomodoro?
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    clearTarget();
    beepAlarm();
    showClock10sThenSleep();
  }

  M5.Lcd.fillScreen(BLACK);
  drawClock(0);
}

// ========== LOOP ==========

void loop() {
  M5.update();
  
  // Update handlers
  btnB.update();
  tapDetector.update();

  // Display remaining time if Pomodoro is armed
  uint32_t tgt = readTarget();
  uint32_t remain = 0;
  if (tgt) {
    uint32_t nowE = rtcEpochNow();
    if (nowE < tgt) {
      remain = tgt - nowE;
    } else {
      clearTarget();
    }
  }

  static unsigned long tRef = 0;
  if (millis() - tRef > CLOCK_REFRESH_MS) {
    drawClock(remain);
    tRef = millis();
  }

  delay(10);
}