#ifndef TIME_SELECTOR_DEPS_H
#define TIME_SELECTOR_DEPS_H

#include "../ports/time_selector_port.h"
#include "../ports/display_handler_port.h"
#include "../adapters/time_selector_m5stick_adapter.h"

inline ITimeSelector* getM5StickTimeSelector(IDisplayHandler* display, const char* title = "Set Time") {
    return new TimeSelectorM5StickAdapter(display, title);
}

#endif
