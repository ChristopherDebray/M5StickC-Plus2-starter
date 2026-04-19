#ifndef MQTT_HELPER_M5STICK_ADAPTER_H
#define MQTT_HELPER_M5STICK_ADAPTER_H

#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <functional>
#include "../ports/mqtt_helper_port.h"

class MQTTHelperM5StickAdapter : public IMQTTHelper {
public:
    MQTTHelperM5StickAdapter()
        : _host(nullptr), _port(1883), _username(nullptr),
          _password(nullptr), _autoReconnect(true) {
        setupDefaultCallbacks();
    }

    void begin(const char* serverHost, uint16_t serverPort = 1883,
               const char* user = nullptr, const char* pass = nullptr) override {
        _host     = serverHost;
        _port     = serverPort;
        _username = user;
        _password = pass;
        _client.setServer(_host, _port);
        if (_username && _password) _client.setCredentials(_username, _password);
        Serial.println("MQTT configured");
    }

    void connect() override {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi not connected. Cannot connect to MQTT.");
            return;
        }
        Serial.print("Connecting to MQTT broker: ");
        Serial.println(_host);
        _client.connect();
    }

    void disconnect() override {
        _client.disconnect();
        Serial.println("MQTT disconnected");
    }

    bool isConnected() override { return _client.connected(); }

    uint16_t publishText(const char* topic, const char* payload,
                         uint8_t qos = 0, bool retain = false) override {
        if (!isConnected()) { Serial.println("Not connected to MQTT"); return 0; }
        return _client.publish(topic, qos, retain, payload);
    }

    uint16_t publishJson(const char* topic, JsonDocument& doc,
                         uint8_t qos = 0, bool retain = false) override {
        char buffer[512];
        size_t len = serializeJson(doc, buffer, sizeof(buffer));
        if (len == 0) { Serial.println("JSON serialization failed"); return 0; }
        return _client.publish(topic, qos, retain, buffer, len);
    }

    uint16_t subscribe(const char* topic, uint8_t qos = 0) override {
        if (!isConnected()) { Serial.println("Not connected to MQTT"); return 0; }
        Serial.print("Subscribing to "); Serial.println(topic);
        return _client.subscribe(topic, qos);
    }

    uint16_t unsubscribe(const char* topic) override {
        return _client.unsubscribe(topic);
    }

    void onConnect(std::function<void(bool)> callback) override {
        _onConnectCallback = callback;
    }

    void onMessage(std::function<void(char*, char*, size_t)> callback) override {
        _onMessageCallback = callback;
    }

    void onDisconnect(std::function<void()> callback) override {
        _onDisconnectCallback = callback;
    }

    void   setAutoReconnect(bool enable) override { _autoReconnect = enable; }
    String getDeviceId() override { return WiFi.macAddress(); }

private:
    AsyncMqttClient _client;
    const char*     _host;
    uint16_t        _port;
    const char*     _username;
    const char*     _password;
    bool            _autoReconnect;

    std::function<void(bool)>                  _onConnectCallback;
    std::function<void(char*, char*, size_t)>  _onMessageCallback;
    std::function<void()>                      _onDisconnectCallback;

    void setupDefaultCallbacks() {
        _client.onConnect([this](bool sessionPresent) {
            Serial.println("Connected to MQTT broker");
            Serial.print(" Session present: ");
            Serial.println(sessionPresent ? "Yes" : "No");
            if (_onConnectCallback) _onConnectCallback(sessionPresent);
        });

        _client.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
            Serial.print("Disconnected from MQTT. Reason: ");
            Serial.println((int)reason);
            if (_onDisconnectCallback) _onDisconnectCallback();
            if (_autoReconnect && WiFi.status() == WL_CONNECTED) {
                Serial.println("Auto-reconnecting in 5 seconds...");
            }
        });

        _client.onMessage([this](char* topic, char* payload,
                                 AsyncMqttClientMessageProperties props,
                                 size_t len, size_t index, size_t total) {
            Serial.print("Message received on ");
            Serial.print(topic); Serial.print(": ");
            char* message = new char[len + 1];
            memcpy(message, payload, len);
            message[len] = '\0';
            Serial.println(message);
            if (_onMessageCallback) _onMessageCallback(topic, message, len);
            delete[] message;
        });
    }
};

#endif
