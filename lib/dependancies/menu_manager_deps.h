#ifndef MENU_MANAGER_DEPS_H
#define MENU_MANAGER_DEPS_H

#include "../ports/menu_manager_port.h"
#include "../ports/display_handler_port.h"
#include "../adapters/menu_manager_m5stick_adapter.h"

inline IMenuManager* getM5StickMenuManager(IDisplayHandler* display) {
    return new MenuManagerM5StickAdapter(display);
}

#endif
