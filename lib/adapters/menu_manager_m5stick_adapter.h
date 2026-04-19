#ifndef MENU_MANAGER_M5STICK_ADAPTER_H
#define MENU_MANAGER_M5STICK_ADAPTER_H

#include "../ports/menu_manager_port.h"
#include "../ports/display_handler_port.h"

#define MAX_MENU_STACK 5

class MenuManagerM5StickAdapter : public IMenuManager {
public:
    MenuManagerM5StickAdapter(IDisplayHandler* disp)
        : _display(disp), _stackSize(0), _isActive(false) {}

    ~MenuManagerM5StickAdapter() {
        while (_stackSize > 0) popMenu();
    }

    bool pushMenu(IMenuHandler* menu) override {
        if (_stackSize >= MAX_MENU_STACK) return false;
        _menuStack[_stackSize++] = menu;
        _isActive = true;
        menu->draw();
        return true;
    }

    bool popMenu() override {
        if (_stackSize <= 0) { _isActive = false; return false; }
        _stackSize--;
        if (_stackSize > 0) {
            _menuStack[_stackSize - 1]->draw();
        } else {
            _isActive = false;
            _display->clearScreen();
        }
        return true;
    }

    void closeAll() override {
        while (_stackSize > 0) popMenu();
        _display->clearScreen();
        _isActive = false;
    }

    void navigateUp() override {
        IMenuHandler* current = getCurrentMenu();
        if (current) { current->moveUp(); current->draw(); }
    }

    void navigateDown() override {
        IMenuHandler* current = getCurrentMenu();
        if (current) { current->moveDown(); current->draw(); }
    }

    void select() override {
        IMenuHandler* current = getCurrentMenu();
        if (current) current->select();
    }

    bool hasActiveMenu() override { return _isActive && _stackSize > 0; }
    int  getStackSize()  override { return _stackSize; }

    IMenuHandler* getCurrentMenu() override {
        return _stackSize > 0 ? _menuStack[_stackSize - 1] : nullptr;
    }

private:
    IDisplayHandler* _display;
    IMenuHandler*    _menuStack[MAX_MENU_STACK];
    int              _stackSize;
    bool             _isActive;
};

#endif
