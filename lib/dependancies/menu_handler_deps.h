#ifndef MENU_DEPS_H
#define MENU_DEPS_H

#include "../ports/menu_handler_port.h"
#include "../adapters/menu_handler_m5stick_adapter.h"
#include "../ports/display_handler_port.h"

inline IMenuHandler* getM5StickMenuHandler(IDisplayHandler* disp, const char* title = "Menu") {
    return new MenuHandlerM5StickAdapter(disp, title);
}

#endif
