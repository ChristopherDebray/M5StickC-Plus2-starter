#ifndef BATTERY_HANDLER_PORT_H
#define BATTERY_HANDLER_PORT_H

#include <stdint.h>

class IBatteryHandler {
public:
    virtual ~IBatteryHandler() = default;

    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void displayInfo() = 0;
    virtual void deepSleep(uint64_t microseconds = 0) = 0;
    virtual void cutAllNonCore() = 0;

    virtual int32_t getCurrent() = 0;
    virtual int32_t getLevel() = 0;
    virtual int16_t getVoltage() = 0;
    virtual bool    isCharging() = 0;
};

#endif
