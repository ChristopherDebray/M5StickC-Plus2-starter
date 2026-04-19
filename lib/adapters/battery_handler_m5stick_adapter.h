#ifndef BATTERY_HANDLER_M5STICK_ADAPTER_H
#define BATTERY_HANDLER_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <driver/gpio.h>
#include "../ports/battery_handler_port.h"
#include "../ports/display_handler_port.h"

#define BUTTON_A_GPIO GPIO_NUM_37
#define WAKEUP_BUTTON_MASK (1ULL << BUTTON_A_GPIO)

class BatteryHandlerM5StickAdapter : public IBatteryHandler {
public:
    BatteryHandlerM5StickAdapter(IDisplayHandler* display, uint32_t intervalMs = 5000)
        : _display(display), _bc(0), _bl(0), _bv(0),
          _ic(m5::Power_Class::is_charging_t::is_discharging),
          _lastUpdate(0), _updateInterval(intervalMs) {}

    void begin() override { update(); }

    void update() override {
        unsigned long now = millis();
        if (now - _lastUpdate < _updateInterval) return;
        _lastUpdate = now;
        _bc = M5.Power.getBatteryCurrent();
        _bl = M5.Power.getBatteryLevel();
        _bv = M5.Power.getBatteryVoltage();
        _ic = M5.Power.isCharging();
    }

    void displayInfo() override {
        _display->displayBatteryLevel(_bl, batteryColor(_bl), isCharging());
    }

    void deepSleep(uint64_t microseconds = 0) override {
        pinMode(4, OUTPUT);
        digitalWrite(4, HIGH);
        gpio_hold_en(GPIO_NUM_4);
        gpio_deep_sleep_hold_en();
        if (microseconds > 0) {
            esp_sleep_enable_timer_wakeup(microseconds);
        }
        esp_sleep_enable_ext1_wakeup(WAKEUP_BUTTON_MASK, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_deep_sleep_start();
    }

    void cutAllNonCore() override {
        esp_wifi_stop();
        btStop();
        M5.Speaker.end();
        M5.Display.sleep();
    }

    int32_t getCurrent() override { return _bc; }
    int32_t getLevel()   override { return _bl; }
    int16_t getVoltage() override { return _bv; }
    bool    isCharging() override {
        return _ic == m5::Power_Class::is_charging_t::is_charging;
    }

private:
    IDisplayHandler*              _display;
    int32_t                       _bc, _bl;
    int16_t                       _bv;
    m5::Power_Class::is_charging_t _ic;
    unsigned long                 _lastUpdate;
    uint32_t                      _updateInterval;

    int batteryColor(int32_t level) {
        if (level > 80) return GREEN;
        if (level > 60) return BLUE;
        if (level > 30) return YELLOW;
        return RED;
    }
};

#endif
