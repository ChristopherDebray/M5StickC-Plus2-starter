#ifndef CLOCK_HANDLER_M5STICK_ADAPTER_H
#define CLOCK_HANDLER_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include <Preferences.h>
#include "../ports/clock_handler_port.h"
#include "../ports/display_handler_port.h"
#include "../ports/battery_handler_port.h"
#include "../ports/rtc_utils_port.h"

class ClockHandlerM5StickAdapter : public IClockHandler {
public:
    static const int POMODORO_MINUTES = 25;
    static const int CLOCK_REFRESH_MS = 1000;

    ClockHandlerM5StickAdapter(IDisplayHandler* display, IBatteryHandler* battery, IRtcUtils* rtc)
        : _display(display), _battery(battery), _rtc(rtc) {
        memset(_timeBuffer, 0, sizeof(_timeBuffer));
        memset(_dateBuffer, 0, sizeof(_dateBuffer));
    }

    void updateDateTime() override { M5.Rtc.getDateTime(&_dt); }

    const char* getCurrentFullTime() override {
        updateDateTime();
        sprintf(_timeBuffer, "%02d:%02d:%02d", _dt.time.hours, _dt.time.minutes, _dt.time.seconds);
        return _timeBuffer;
    }

    const char* getCurrentFullDateFR() override {
        updateDateTime();
        sprintf(_dateBuffer, "%02d-%02d-%04d", _dt.date.date, _dt.date.month, _dt.date.year);
        return _dateBuffer;
    }

    const char* getCurrentFullDateUS() override {
        updateDateTime();
        sprintf(_dateBuffer, "%02d-%02d-%04d", _dt.date.month, _dt.date.date, _dt.date.year);
        return _dateBuffer;
    }

    const char* getCurrentFullDateISO() override {
        updateDateTime();
        sprintf(_dateBuffer, "%04d-%02d-%02d", _dt.date.year, _dt.date.month, _dt.date.date);
        return _dateBuffer;
    }

    void drawClock(uint32_t remainSec = 0) override {
        static uint32_t lastRemain = 9999;
        static uint8_t  lastSec    = 99;
        updateDateTime();
        if (_dt.time.seconds != lastSec || remainSec != lastRemain) {
            _display->clearScreen();
            _display->displayMainTitle(getCurrentFullTime());
            _display->displaySubtitle(getCurrentFullDateFR());
            if (remainSec > 0) {
                char pomoText[32];
                sprintf(pomoText, "Pomo: %02u:%02u", remainSec / 60, remainSec % 60);
                _display->displayInfoMessage(pomoText);
            }
            lastSec    = _dt.time.seconds;
            lastRemain = remainSec;
        }
    }

    void armPomodoroAndSleep() override {
        _display->clearScreen();
        _display->displayMainTitle("Pomodoro", MSG_SUCCESS);
        delay(1500);
        armTimerAndSleep(POMODORO_MINUTES);
    }

    void armTimerAndSleep(uint32_t minutes) override {
        uint32_t target = _rtc->epochNow() + minutes * 60;
        writeTarget(target);
        _battery->deepSleep((uint64_t)minutes * 60ULL * 1000000ULL);
    }

    void writeTarget(uint32_t epoch) override {
        _prefs.begin("clock", false);
        _prefs.putUInt("target", epoch);
        _prefs.end();
    }

    uint32_t readTarget() override {
        _prefs.begin("clock", true);
        uint32_t v = _prefs.getUInt("target", 0);
        _prefs.end();
        return v;
    }

    void clearTarget() override { writeTarget(0); }

    int getHours()   override { updateDateTime(); return _dt.time.hours;   }
    int getMinutes() override { updateDateTime(); return _dt.time.minutes; }
    int getSeconds() override { updateDateTime(); return _dt.time.seconds; }
    int getYear()    override { updateDateTime(); return _dt.date.year;    }
    int getMonth()   override { updateDateTime(); return _dt.date.month;   }
    int getDay()     override { updateDateTime(); return _dt.date.date;    }
    int getWeekDay() override { updateDateTime(); return _dt.date.weekDay; }

private:
    IDisplayHandler*   _display;
    IBatteryHandler*   _battery;
    IRtcUtils*         _rtc;
    m5::rtc_datetime_t _dt;
    char               _timeBuffer[16];
    char               _dateBuffer[16];
    Preferences        _prefs;
};

#endif
