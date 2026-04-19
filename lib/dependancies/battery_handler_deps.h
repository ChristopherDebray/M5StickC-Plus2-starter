#ifndef BATTERY_HANDLER_DEPS_H
#define BATTERY_HANDLER_DEPS_H

#include "../ports/battery_handler_port.h"
#include "../ports/display_handler_port.h"
#include "../adapters/battery_handler_m5stick_adapter.h"

inline IBatteryHandler* getM5StickBatteryHandler(IDisplayHandler* display, uint32_t intervalMs = 5000) {
    return new BatteryHandlerM5StickAdapter(display, intervalMs);
}

#endif
