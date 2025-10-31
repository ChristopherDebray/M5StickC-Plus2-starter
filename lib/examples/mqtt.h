#ifndef MQTT_EXAMPLE_USAGE_H
#define MQTT_EXAMPLE_USAGE_H

#include "../mqtt_helper.h"
#include "../wifi_helper.h"
#include "../display_handler.h"
#include "../battery_handler.h"

// Instance globale
MQTTHelper mqtt;
DisplayHandler displayHandler;
BatteryHandler batteryHandler(&displayHandler);

// ═══════════════════════════════════════════════════════════
// EXEMPLE 1 : Config + simple connection
// ═══════════════════════════════════════════════════════════

void setupMQTT() {
    // 1. Connect to wifi first
    if (!WiFiHelper::connect("VOTRE_SSID", "VOTRE_PASSWORD")) {
        Serial.println("❌ WiFi connection failed");
        return;
    }
    
    // 2. config MQTT
    mqtt.begin(
        "mqtt.example.com",  // Host
        1883,                // port (default: 1883)
        "username",          // Username (optional)
        "password"           // Password (optional)
    );
    
    // 3. define callbacks (optional)
    mqtt.onConnect([](bool sessionPresent) {
        Serial.println("Connected,now subscribing to topics");
        mqtt.subscribe("home/temperature");
        mqtt.subscribe("home/commands");
    });
    
    mqtt.onMessage([](char* topic, char* payload, size_t len) {
        Serial.print("New message on: ");
        Serial.print(topic);
        Serial.print(": ");
        Serial.println(payload);
        
        // Parse json
        StaticJsonDocument<256> doc;
        DeserializationError err = deserializeJson(doc, payload, len);
        
        if (!err) {
            const char* command = doc["command"];
            Serial.print("Commande: ");
            Serial.println(command);
        }
    });
    
    // 4. Connect to the broker 
    mqtt.connect();
}

// ═══════════════════════════════════════════════════════════
// EXEMPLE 2 : Publish sensors datas
// ═══════════════════════════════════════════════════════════

void publishSensorData() {
    if (!mqtt.isConnected()) {
        Serial.println("⚠️ MQTT not connected");
        return;
    }
    
    // 1. Creates a json
    // This is comming from a previous personal project, but the key->value can be whatever you need them to be
    StaticJsonDocument<256> doc;
    doc["device_id"] = MQTTHelper::getDeviceId();
    doc["temperature"] = 23.5;
    doc["humidity"] = 65.2;
    doc["battery"] = 87;
    doc["timestamp"] = millis();
    
    // 2. publish
    mqtt.publishJson("sensors/data", doc, 1);  // QoS 1
}

// ═══════════════════════════════════════════════════════════
// EXEMPLE 3 : Publish simple message
// ═══════════════════════════════════════════════════════════

void publishSimpleMessage() {
    mqtt.publishText("home/status", "Device is online");
}

// ═══════════════════════════════════════════════════════════
// EXEMPLE 4 : listen commands
// ═══════════════════════════════════════════════════════════

void setupCommandListener() {
    // 1. Subscribe to a specific topic to the device with getDeviceId
    String commandTopic = "device/" + MQTTHelper::getDeviceId() + "/commands";
    
    mqtt.onConnect([commandTopic](bool sessionPresent) {
        mqtt.subscribe(commandTopic.c_str(), 1);
    });
    
    // 2. Listen to RECEIVED message
    mqtt.onMessage([](char* topic, char* payload, size_t len) {
        StaticJsonDocument<256> doc;
        DeserializationError err = deserializeJson(doc, payload, len);
        
        if (err) {
            Serial.println("Invalid JSON command");
            return;
        }
        
        const char* action = doc["action"];
        
        if (strcmp(action, "restart") == 0) {
            Serial.println("Restarting device");
            delay(1000);
            ESP.restart();
        }
        else if (strcmp(action, "sleep") == 0) {
            Serial.println("Going to sleep");
            batteryHandler.M5deepSleep();
        }
        else if (strcmp(action, "set_config") == 0) {
            int userId = doc["user_id"];
            int zoneId = doc["zone_id"];
            
            Serial.printf("Config updated: User=%d, Zone=%d\n", userId, zoneId);
            
            // Save in preferences
            // settings->setUserId(userId);
            // settings->setZoneId(zoneId);
        }
    });
}

// ═══════════════════════════════════════════════════════════
// EXEMPLE 5 : Full use in main
// ═══════════════════════════════════════════════════════════

/*
void setup() {
    Serial.begin(115200);
    
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Setup WiFi + MQTT
    setupMQTT();
    setupCommandListener();
}

void loop() {
    M5.update();
    
    // Send data every 10s
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish > 10000) {
        publishSensorData();
        lastPublish = millis();
    }
    
    delay(10);
}
*/

// ═══════════════════════════════════════════════════════════
// EXEMPLE 6 : Handle auto reconnect
// ═══════════════════════════════════════════════════════════

void setupWithAutoReconnect() {
    mqtt.setAutoReconnect(true);
    
    mqtt.onDisconnect([]() {
        Serial.println("⚠️ MQTT disconnected!");
        
        // wait 5s then reconnect
        delay(5000);
        if (WiFiHelper::isConnected()) {
            mqtt.connect();
        }
    });
}

// ═══════════════════════════════════════════════════════════
// EXEMPLE 7 : Publish with Retain and QoS
// ═══════════════════════════════════════════════════════════

void publishWithOptions() {
    // QoS 0 = Fire and forget
    mqtt.publishText("status/online", "true", 0, false);
    
    // QoS 1 = At least once delivery
    mqtt.publishText("status/battery", "87%", 1, false);
    
    // QoS 2 = Exactly once delivery (safest but slowest)
    // mqtt.publish("critical/alert", "Low battery!", 2, false);
    
    // Retain = broker keeps the last message
    mqtt.publishText("status/last_seen", "2025-01-01 12:34:56", 0, true);
}

#endif