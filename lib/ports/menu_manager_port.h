#ifndef MENU_MANAGER_PORT_H
#define MENU_MANAGER_PORT_H

#include "./menu_handler_port.h"

class IMenuManager {
public:
    virtual ~IMenuManager() = default;

    virtual bool pushMenu(IMenuHandler* menu) = 0;
    virtual bool popMenu() = 0;
    virtual void closeAll() = 0;
    virtual void navigateUp() = 0;
    virtual void navigateDown() = 0;
    virtual void select() = 0;
    virtual bool hasActiveMenu() = 0;
    virtual int  getStackSize() = 0;
    virtual IMenuHandler* getCurrentMenu() = 0;
};

#endif
