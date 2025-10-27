#ifndef TAP_HANDLER_H
#define TAP_HANDLER_H

#include <M5StickCPlus2.h>
#include <Arduino.h>

// Tap detection handler with callback
class TapHandler {
private:
  float lastAccel;
  unsigned long lastTapTime;
  int tapCount;
  
  float tapThreshold;
  uint32_t tapTimeout;
  int maxTaps;
  
  void (*onTapsCallback)(int count);
  
public:
  // Constructor
  TapHandler(float threshold = 1.5, uint32_t timeoutMs = 300, int maxTapCount = 3) {
    lastAccel = 0;
    lastTapTime = 0;
    tapCount = 0;
    tapThreshold = threshold;
    tapTimeout = timeoutMs;
    maxTaps = maxTapCount;
    onTapsCallback = nullptr;
  }
  
  // Initialize IMU
  void begin() {
    M5.Imu.begin();
  }
  
  // Set callback for when tap sequence is detected
  void onTaps(void (*callback)(int count)) {
    onTapsCallback = callback;
  }
  
  // Update function - call this in loop()
  void update() {
    float accX, accY, accZ;
    M5.Imu.getAccel(&accX, &accY, &accZ);
    
    // Calculate total acceleration magnitude
    float accelMagnitude = sqrt(accX * accX + accY * accY + accZ * accZ);
    
    // Detect sudden change (tap/shock)
    float accelDelta = abs(accelMagnitude - lastAccel);
    lastAccel = accelMagnitude;
    
    unsigned long now = millis();
    
    // Tap detected!
    if (accelDelta > tapThreshold) {
      // Check if this is part of a sequence
      if (now - lastTapTime < tapTimeout) {
        tapCount++;
      } else {
        tapCount = 1;  // First tap of new sequence
      }
      
      lastTapTime = now;
      
      // Check if we reached max taps
      if (tapCount >= maxTaps) {
        if (onTapsCallback != nullptr) {
          onTapsCallback(tapCount);
        }
        tapCount = 0;  // Reset
      }
    }
    
    // Reset tap count if timeout
    if (now - lastTapTime > tapTimeout && tapCount > 0) {
      // Could add callback for incomplete sequences here if needed
      tapCount = 0;
    }
  }
  
  // Get current tap count (for debugging)
  int getTapCount() {
    return tapCount;
  }
};

#endif