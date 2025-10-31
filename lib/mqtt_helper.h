#ifndef MQTT_HELPER_H
#define MQTT_HELPER_H

#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <functional>

class MQTTHelper
{
private:
    AsyncMqttClient client;

    const char *host;
    uint16_t port;
    const char *username;
    const char *password;

    bool autoReconnect;

    std::function<void(bool sessionPresent)> onConnectCallback;
    std::function<void(char *topic, char *payload, size_t len)> onMessageCallback;
    std::function<void()> onDisconnectCallback;

public:
    MQTTHelper() : host(nullptr), port(1883), username(nullptr),
                   password(nullptr), autoReconnect(true)
    {
        setupDefaultCallbacks();
    }

    // MQTT server config
    void begin(const char *serverHost, uint16_t serverPort = 1883,
               const char *user = nullptr, const char *pass = nullptr)
    {
        host = serverHost;
        port = serverPort;
        username = user;
        password = pass;

        client.setServer(host, port);

        if (username && password)
        {
            client.setCredentials(username, password);
        }

        Serial.println("MQTT configured");
    }

    void connect()
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi not connected. Cannot connect to MQTT.");
            return;
        }

        Serial.print("Connecting to MQTT broker: ");
        Serial.println(host);
        client.connect();
    }

    void disconnect()
    {
        client.disconnect();
        Serial.println("MQTT disconnected");
    }

    bool isConnected()
    {
        return client.connected();
    }

    uint16_t publishText(const char *topic, const char *payload,
                         uint8_t qos = 0, bool retain = false)
    {
        if (!isConnected())
        {
            Serial.println("Not connected to MQTT");
            return 0;
        }

        return client.publish(topic, qos, retain, payload);
    }

    uint16_t publishJson(const char *topic, JsonDocument &doc,
                         uint8_t qos = 0, bool retain = false)
    {
        char buffer[512];
        size_t len = serializeJson(doc, buffer, sizeof(buffer));

        if (len == 0)
        {
            Serial.println("JSON serialization failed");
            return 0;
        }

        return client.publish(topic, qos, retain, buffer, len);
    }

    uint16_t subscribe(const char *topic, uint8_t qos = 0)
    {
        if (!isConnected())
        {
            Serial.println("Not connected to MQTT");
            return 0;
        }

        Serial.print("Subscribing to ");
        Serial.println(topic);

        return client.subscribe(topic, qos);
    }

    uint16_t unsubscribe(const char *topic)
    {
        return client.unsubscribe(topic);
    }

    void onConnect(std::function<void(bool)> callback)
    {
        onConnectCallback = callback;
    }

    void onMessage(std::function<void(char *, char *, size_t)> callback)
    {
        onMessageCallback = callback;
    }

    void onDisconnect(std::function<void()> callback)
    {
        onDisconnectCallback = callback;
    }

    void setAutoReconnect(bool enable)
    {
        autoReconnect = enable;
    }

    static String getDeviceId()
    {
        return WiFi.macAddress();
    }

private:
    void setupDefaultCallbacks()
    {
        client.onConnect([this](bool sessionPresent)
                         {
            Serial.println("Connected to MQTT broker");
            Serial.print(" Sessin present: ");
            Serial.println(sessionPresent ? "Yes" : "No");
            
            if (onConnectCallback) {
                onConnectCallback(sessionPresent);
            } });

        client.onDisconnect([this](AsyncMqttClientDisconnectReason reason)
                            {
            Serial.print("Disconnected from MQTT. Reason: ");
            Serial.println((int)reason);
            
            if (onDisconnectCallback) {
                onDisconnectCallback();
            }
            
            if (autoReconnect && WiFi.status() == WL_CONNECTED) {
                Serial.println("Auto-reconnecting in 5 seconds...");
                // In a "real" project you should used a timer
            } });

        // Callback on RECEIVED message
        client.onMessage([this](char *topic, char *payload,
                                AsyncMqttClientMessageProperties props,
                                size_t len, size_t index, size_t total)
                         {
            Serial.print("Message received on ");
            Serial.print(topic);
            Serial.print(": ");
            
            char* message = new char[len + 1];
            memcpy(message, payload, len);
            message[len] = '\0';
            
            Serial.println(message);
            
            if (onMessageCallback) {
                onMessageCallback(topic, message, len);
            }
            
            delete[] message; });

        // Callback of PUBLISH success, uncoment to print message id
        // client.onPublish([](uint16_t packetId) {
        //     Serial.print("Message published (ID: ");
        //     Serial.print(packetId);
        //     Serial.println(")");
        // });
    }
};

#endif