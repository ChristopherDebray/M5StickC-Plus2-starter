#ifndef MQTT_HELPER_PORT_H
#define MQTT_HELPER_PORT_H

#include <stdint.h>
#include <functional>
#include <ArduinoJson.h>
#include <Arduino.h>

class IMQTTHelper {
public:
    virtual ~IMQTTHelper() = default;

    virtual void begin(const char* serverHost, uint16_t serverPort = 1883,
                       const char* user = nullptr, const char* pass = nullptr) = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;

    virtual uint16_t publishText(const char* topic, const char* payload,
                                 uint8_t qos = 0, bool retain = false) = 0;
    virtual uint16_t publishJson(const char* topic, JsonDocument& doc,
                                 uint8_t qos = 0, bool retain = false) = 0;
    virtual uint16_t subscribe(const char* topic, uint8_t qos = 0) = 0;
    virtual uint16_t unsubscribe(const char* topic) = 0;

    virtual void onConnect(std::function<void(bool)> callback) = 0;
    virtual void onMessage(std::function<void(char*, char*, size_t)> callback) = 0;
    virtual void onDisconnect(std::function<void()> callback) = 0;

    virtual void   setAutoReconnect(bool enable) = 0;
    virtual String getDeviceId() = 0;
};

#endif
