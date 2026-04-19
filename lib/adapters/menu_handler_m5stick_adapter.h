#ifndef MENU_HANDLER_M5STICK_ADAPTER_H
#define MENU_HANDLER_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include <functional>
#include "../ports/menu_handler_port.h"
#include "../display_handler.h"
#include "../settings_manager.h"

#define MAX_MENU_ITEMS 10

struct MenuItem {
    const char* label;
    std::function<void()> callback;
    bool enabled;

    MenuItem() : label(""), callback(nullptr), enabled(true) {}
    MenuItem(const char* lbl, std::function<void()> cb) : label(lbl), callback(cb), enabled(true) {}
};

class MenuHandlerM5StickAdapter : public IMenuHandler {
public:
    MenuHandlerM5StickAdapter(DisplayHandler* disp, const char* menuTitle = "Menu")
        : _display(disp), _title(menuTitle),
          _itemCount(0), _selectedIndex(0), _scrollOffset(0), _maxVisibleItems(4) {}

    bool addItem(const char* label, std::function<void()> callback) override {
        if (_itemCount >= MAX_MENU_ITEMS) return false;
        _items[_itemCount++] = MenuItem(label, callback);
        return true;
    }

    void clear() override {
        _itemCount = 0;
        _selectedIndex = 0;
        _scrollOffset = 0;
    }

    void moveUp() override {
        if (_selectedIndex > 0) {
            _selectedIndex--;
            if (_selectedIndex < _scrollOffset) {
                _scrollOffset = _selectedIndex;
            }
            if (SettingsManager::getInstance()->getUiSound()) {
                M5.Speaker.tone(2000, 30);
            }
        }
    }

    void moveDown() override {
        if (_selectedIndex < _itemCount - 1) {
            _selectedIndex++;
            if (_selectedIndex >= _scrollOffset + _maxVisibleItems) {
                _scrollOffset = _selectedIndex - _maxVisibleItems + 1;
            }
            if (SettingsManager::getInstance()->getUiSound()) {
                M5.Speaker.tone(2000, 30);
            }
        }
    }

    void select() override {
        if (_selectedIndex >= 0 && _selectedIndex < _itemCount) {
            MenuItem& item = _items[_selectedIndex];
            if (item.enabled && item.callback != nullptr) {
                if (SettingsManager::getInstance()->getUiSound()) {
                    M5.Speaker.tone(2500, 50);
                    delay(50);
                    M5.Speaker.tone(3000, 50);
                }
                item.callback();
            }
        }
    }

    void draw() override {
        _display->clearScreen();
        _display->displayTextAt(_title, 10, 5, 1, MSG_INFO);

        if (_scrollOffset > 0) {
            _display->displayTextAt("^", 220, 5, 1, MSG_NORMAL);
        }

        for (int i = 0; i < _maxVisibleItems && (_scrollOffset + i) < _itemCount; i++) {
            int idx = _scrollOffset + i;
            MenuItem& item = _items[idx];
            int y = START_Y + (i * ITEM_HEIGHT);

            if (idx == _selectedIndex) {
                M5.Display.fillRect(0, y - 2, 240, ITEM_HEIGHT, DARKGREY);
                _display->displayTextAt(">", 5, y, TEXT_SIZE, MSG_SUCCESS);
            }

            MessageType color = item.enabled ? MSG_NORMAL : MSG_ERROR;
            _display->displayTextAt(item.label, 25, y, TEXT_SIZE, color);
        }

        if (_scrollOffset + _maxVisibleItems < _itemCount) {
            _display->displayTextAt("v", 220, 120, 1, MSG_NORMAL);
        }

        char counter[16];
        sprintf(counter, "%d/%d", _selectedIndex + 1, _itemCount);
        _display->displayTextAt(counter, 180, 5, 1, MSG_NORMAL);
    }

    void setItemEnabled(int index, bool enabled) override {
        if (index >= 0 && index < _itemCount) {
            _items[index].enabled = enabled;
        }
    }

    int getSelectedIndex() override { return _selectedIndex; }
    int getItemCount() override     { return _itemCount; }

    const char* getSelectedLabel() override {
        if (_selectedIndex >= 0 && _selectedIndex < _itemCount) {
            return _items[_selectedIndex].label;
        }
        return "";
    }

    void resetSelection() override {
        _selectedIndex = 0;
        _scrollOffset = 0;
    }

private:
    DisplayHandler* _display;
    const char*     _title;
    MenuItem        _items[MAX_MENU_ITEMS];
    int             _itemCount;
    int             _selectedIndex;
    int             _scrollOffset;
    int             _maxVisibleItems;

    static const int ITEM_HEIGHT = 20;
    static const int START_Y     = 30;
    static const int TEXT_SIZE   = 2;
};

#endif
