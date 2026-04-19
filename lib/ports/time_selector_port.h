#ifndef TIME_SELECTOR_PORT_H
#define TIME_SELECTOR_PORT_H

#include <stdint.h>
#include <functional>

struct TimeValue {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    TimeValue() : hours(0), minutes(0), seconds(0) {}
    TimeValue(uint8_t h, uint8_t m, uint8_t s) : hours(h), minutes(m), seconds(s) {}
};

class ITimeSelector {
public:
    virtual ~ITimeSelector() = default;

    virtual void configureSeconds(uint8_t defaultSec = 15, uint8_t minSec = 5, uint8_t maxSec = 60) = 0;
    virtual void configureHoursMinutes(uint8_t defaultHour = 0, uint8_t defaultMin = 0) = 0;
    virtual void configureHoursMinutesSeconds(uint8_t h = 0, uint8_t m = 0, uint8_t s = 0) = 0;

    virtual void setOnComplete(std::function<void(TimeValue)> callback) = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isActive() = 0;

    virtual void navigateUp() = 0;
    virtual void navigateDown() = 0;
    virtual void select() = 0;
    virtual void draw() = 0;
};

#endif
