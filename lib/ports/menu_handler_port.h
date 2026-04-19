#ifndef MENU_HANDLER_PORT_H
#define MENU_HANDLER_PORT_H

#include <functional>

class IMenuHandler {
public:
    virtual ~IMenuHandler() = default;

    virtual bool addItem(const char* label, std::function<void()> callback) = 0;
    virtual void clear() = 0;

    virtual void moveUp() = 0;
    virtual void moveDown() = 0;
    virtual void select() = 0;

    virtual void draw() = 0;

    virtual void setItemEnabled(int index, bool enabled) = 0;

    virtual int getSelectedIndex() = 0;
    virtual int getItemCount() = 0;
    virtual const char* getSelectedLabel() = 0;

    virtual void resetSelection() = 0;
};

#endif
