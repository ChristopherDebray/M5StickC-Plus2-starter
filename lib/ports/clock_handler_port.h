#ifndef CLOCK_HANDLER_PORT_H
#define CLOCK_HANDLER_PORT_H

#include <stdint.h>

class IClockHandler {
public:
    virtual ~IClockHandler() = default;

    virtual void updateDateTime() = 0;
    virtual const char* getCurrentFullTime() = 0;
    virtual const char* getCurrentFullDateFR() = 0;
    virtual const char* getCurrentFullDateUS() = 0;
    virtual const char* getCurrentFullDateISO() = 0;

    virtual void drawClock(uint32_t remainSec = 0) = 0;
    virtual void armPomodoroAndSleep() = 0;
    virtual void armTimerAndSleep(uint32_t minutes) = 0;
    virtual void writeTarget(uint32_t epoch) = 0;
    virtual uint32_t readTarget() = 0;
    virtual void clearTarget() = 0;

    virtual int getHours() = 0;
    virtual int getMinutes() = 0;
    virtual int getSeconds() = 0;
    virtual int getYear() = 0;
    virtual int getMonth() = 0;
    virtual int getDay() = 0;
    virtual int getWeekDay() = 0;
};

#endif
