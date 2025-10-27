#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>

// Button handler class with callback support
class ButtonHandler {
private:
  uint32_t lastPressTime;
  bool waitingSecondPress;
  uint32_t doublePressDuration;
  void (*onDoubleClickCallback)();
  
  // Which button to monitor
  enum ButtonType { BTN_A, BTN_B, BTN_C } buttonType;
  
  bool isButtonPressed() {
    switch(buttonType) {
      case BTN_A: return M5.BtnA.wasPressed();
      case BTN_B: return M5.BtnB.wasPressed();
      case BTN_C: return M5.BtnC.wasPressed();
      default: return false;
    }
  }

public:
  // Constructor
  ButtonHandler(int btnType = 1, uint32_t doubleClickMs = 400) {
    lastPressTime = 0;
    waitingSecondPress = false;
    doublePressDuration = doubleClickMs;
    onDoubleClickCallback = nullptr;
    
    // Set button type (0=A, 1=B, 2=C)
    if (btnType == 0) buttonType = BTN_A;
    else if (btnType == 1) buttonType = BTN_B;
    else buttonType = BTN_C;
  }
  
  // Set callback function
  void onDoubleClick(void (*callback)()) {
    onDoubleClickCallback = callback;
  }
  
  // Update function - call this in loop()
  void update() {
    if (isButtonPressed()) {
      uint32_t now = millis();
      
      if (!waitingSecondPress) {
        // First press
        waitingSecondPress = true;
        lastPressTime = now;
      } else {
        // Second press
        if (now - lastPressTime <= doublePressDuration) {
          // Double click detected!
          waitingSecondPress = false;
          if (onDoubleClickCallback != nullptr) {
            onDoubleClickCallback();
          }
        } else {
          // Too slow, reset
          lastPressTime = now;
        }
      }
    }
    
    // Timeout check
    if (waitingSecondPress && (millis() - lastPressTime > doublePressDuration)) {
      waitingSecondPress = false;
    }
  }
};

#endif