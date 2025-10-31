#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <WiFi.h>
#include <Arduino.h>

// Structure pour les credentials
struct WifiCredentials {
    const char* ssid;
    const char* password;
};

class WiFiHelper {
private:
    static const unsigned long CONNECTION_TIMEOUT = 10000; // 10 secondes
    
public:
    // WiFi connect with timeout
    static bool connect(const char* ssid, const char* password) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        delay(500);
        
        Serial.println("Connecting to WiFi...");
        WiFi.begin(ssid, password);
        
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && 
               millis() - startTime < CONNECTION_TIMEOUT) {
            Serial.print(".");
            delay(500);
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n WiFi Connected!");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            return true;
        } else {
            Serial.println("\n WiFi Connection Failed");
            return false;
        }
    }
    
    static void disconnect() {
        WiFi.disconnect(true);
        Serial.println("WiFi Disconnected");
    }
    
    static bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
    
    static String getIP() {
        if (isConnected()) {
            return WiFi.localIP().toString();
        }
        return "Not Connected";
    }
    
    static int getSignalStrength() {
        if (isConnected()) {
            return WiFi.RSSI();
        }
        return 0;
    }
};

#endif