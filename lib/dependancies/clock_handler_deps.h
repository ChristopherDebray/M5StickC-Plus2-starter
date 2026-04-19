#ifndef CLOCK_HANDLER_DEPS_H
#define CLOCK_HANDLER_DEPS_H

#include "../ports/clock_handler_port.h"
#include "../ports/display_handler_port.h"
#include "../ports/battery_handler_port.h"
#include "../ports/rtc_utils_port.h"
#include "../adapters/clock_handler_m5stick_adapter.h"

inline IClockHandler* getM5StickClockHandler(IDisplayHandler* display, IBatteryHandler* battery, IRtcUtils* rtc) {
    return new ClockHandlerM5StickAdapter(display, battery, rtc);
}

#endif
