#ifndef TIME_SELECTOR_M5STICK_ADAPTER_H
#define TIME_SELECTOR_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include <functional>
#include "../ports/time_selector_port.h"
#include "../ports/display_handler_port.h"
#include "../settings_manager.h"

class TimeSelectorM5StickAdapter : public ITimeSelector {
public:
    TimeSelectorM5StickAdapter(IDisplayHandler* disp, const char* titleText = "Set Time")
        : _display(disp), _currentFieldIndex(0), _active(false),
          _virtualMenuSize(0), _virtualCurrentIndex(0),
          _fields(nullptr), _fieldCount(0) {
        _settings = SettingsManager::getInstance();
        _title = String(titleText);
    }

    ~TimeSelectorM5StickAdapter() {
        if (_fields) delete[] _fields;
    }

    void configureSeconds(uint8_t defaultSec = 15, uint8_t minSec = 5, uint8_t maxSec = 60) override {
        if (_fields) delete[] _fields;
        _fieldCount = 1;
        _fields = new TimeFieldConfig[1];
        _fields[0] = {FIELD_SECONDS, "Seconds", minSec, maxSec, defaultSec};
        _currentValue.seconds = defaultSec;
        _currentFieldIndex = 0;
    }

    void configureHoursMinutes(uint8_t defaultHour = 0, uint8_t defaultMin = 0) override {
        if (_fields) delete[] _fields;
        _fieldCount = 2;
        _fields = new TimeFieldConfig[2];
        _fields[0] = {FIELD_HOURS,   "Hours",   0, 23, defaultHour};
        _fields[1] = {FIELD_MINUTES, "Minutes", 0, 59, defaultMin};
        _currentValue.hours   = defaultHour;
        _currentValue.minutes = defaultMin;
        _currentFieldIndex = 0;
    }

    void configureHoursMinutesSeconds(uint8_t h = 0, uint8_t m = 0, uint8_t s = 0) override {
        if (_fields) delete[] _fields;
        _fieldCount = 3;
        _fields = new TimeFieldConfig[3];
        _fields[0] = {FIELD_HOURS,    "Hours",   0, 23, h};
        _fields[1] = {FIELD_MINUTES,  "Minutes", 0, 59, m};
        _fields[2] = {FIELD_SECONDS,  "Seconds", 0, 59, s};
        _currentValue.hours   = h;
        _currentValue.minutes = m;
        _currentValue.seconds = s;
        _currentFieldIndex = 0;
    }

    void setOnComplete(std::function<void(TimeValue)> callback) override {
        _onComplete = callback;
    }

    void start() override {
        _active = true;
        _currentFieldIndex = 0;
        TimeFieldConfig& field = _fields[_currentFieldIndex];
        uint8_t* val = valuePtr(field.field);
        _virtualMenuSize    = field.maxValue - field.minValue + 1;
        _virtualCurrentIndex = *val - field.minValue;
        draw();
    }

    void stop() override { _active = false; }
    bool isActive() override { return _active; }

    void navigateUp() override {
        if (!_active || _fieldCount == 0) return;
        TimeFieldConfig& field = _fields[_currentFieldIndex];
        uint8_t* val = valuePtr(field.field);
        if (--_virtualCurrentIndex < 0) _virtualCurrentIndex = _virtualMenuSize - 1;
        *val = field.minValue + _virtualCurrentIndex;
        if (_settings->getUiSound()) M5.Speaker.tone(2800, 30);
        draw();
    }

    void navigateDown() override {
        if (!_active || _fieldCount == 0) return;
        TimeFieldConfig& field = _fields[_currentFieldIndex];
        uint8_t* val = valuePtr(field.field);
        if (++_virtualCurrentIndex >= _virtualMenuSize) _virtualCurrentIndex = 0;
        *val = field.minValue + _virtualCurrentIndex;
        if (_settings->getUiSound()) M5.Speaker.tone(2400, 30);
        draw();
    }

    void select() override {
        if (!_active || _fieldCount == 0) return;
        if (_settings->getUiSound()) M5.Speaker.tone(3000, 50);
        _currentFieldIndex++;
        if (_currentFieldIndex >= _fieldCount) {
            _active = false;
            if (_onComplete) _onComplete(_currentValue);
        } else {
            TimeFieldConfig& field = _fields[_currentFieldIndex];
            uint8_t* val = valuePtr(field.field);
            _virtualMenuSize     = field.maxValue - field.minValue + 1;
            _virtualCurrentIndex = *val - field.minValue;
            draw();
        }
    }

    void draw() override {
        if (!_active || _fieldCount == 0) return;
        _display->clearScreen();

        TimeFieldConfig& field = _fields[_currentFieldIndex];
        uint8_t* val = valuePtr(field.field);

        _display->drawCenteredText(_title.c_str(), 10, TFT_WHITE, 2);

        char progress[16];
        sprintf(progress, "%d/%d", _currentFieldIndex + 1, _fieldCount);
        M5.Display.setTextColor(TFT_DARKGREY);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(M5.Display.width() - 30, 10);
        M5.Display.print(progress);

        uint8_t prevVal = (*val == field.minValue) ? field.maxValue : (*val - 1);
        char prevStr[8]; sprintf(prevStr, "%02d", prevVal);
        _display->drawCenteredText(prevStr, VALUE_Y - 20, TFT_DARKGREY, 2);

        char valueStr[8]; sprintf(valueStr, "%02d", *val);
        _display->drawCenteredText(valueStr, VALUE_Y + 10, TFT_YELLOW, 4);

        uint8_t nextVal = (*val == field.maxValue) ? field.minValue : (*val + 1);
        char nextStr[8]; sprintf(nextStr, "%02d", nextVal);
        _display->drawCenteredText(nextStr, VALUE_Y + 50, TFT_DARKGREY, 2);

        _display->drawCenteredText(field.label, LABEL_Y, TFT_CYAN, 1);

        M5.Display.setTextColor(TFT_DARKGREY);
        M5.Display.setTextSize(1);
        M5.Display.setCursor(5, M5.Display.height() - 15);
        M5.Display.print("PWR/B:Nav A:OK");
    }

private:
    enum TimeField { FIELD_HOURS, FIELD_MINUTES, FIELD_SECONDS };

    struct TimeFieldConfig {
        TimeField   field;
        const char* label;
        uint8_t     minValue;
        uint8_t     maxValue;
        uint8_t     defaultValue;
    };

    static const int VALUE_Y = 50;
    static const int LABEL_Y = 90;

    IDisplayHandler*              _display;
    SettingsManager*              _settings;
    TimeFieldConfig*              _fields;
    uint8_t                       _fieldCount;
    uint8_t                       _currentFieldIndex;
    TimeValue                     _currentValue;
    std::function<void(TimeValue)> _onComplete;
    String                        _title;
    bool                          _active;
    int                           _virtualMenuSize;
    int                           _virtualCurrentIndex;

    uint8_t* valuePtr(TimeField field) {
        switch(field) {
            case FIELD_HOURS:   return &_currentValue.hours;
            case FIELD_MINUTES: return &_currentValue.minutes;
            default:            return &_currentValue.seconds;
        }
    }
};

#endif
