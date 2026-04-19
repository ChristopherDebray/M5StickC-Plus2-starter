#ifndef DISPLAY_HANDLER_DEPS_H
#define DISPLAY_HANDLER_DEPS_H

#include "../ports/display_handler_port.h"
#include "../adapters/display_handler_m5stick_adapter.h"

inline IDisplayHandler* getM5StickDisplayHandler() {
    return new DisplayHandlerM5StickAdapter();
}

#endif
