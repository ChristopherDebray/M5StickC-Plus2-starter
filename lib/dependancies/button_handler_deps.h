#ifndef BUTTON_HANDLER_DEPS_H
#define BUTTON_HANDLER_DEPS_H

#include "../ports/button_handler_port.h"
#include "../adapters/button_handler_m5stick_adapter.h"

inline IButtonHandler* getM5StickButtonHandler(int btnType = 1, uint32_t doubleClickMs = 400) {
    return new ButtonHandlerM5StickAdapter(btnType, doubleClickMs);
}

#endif
