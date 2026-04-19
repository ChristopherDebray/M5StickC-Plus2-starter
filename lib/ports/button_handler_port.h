#ifndef BUTTON_HANDLER_PORT_H
#define BUTTON_HANDLER_PORT_H

class IButtonHandler {
public:
    virtual ~IButtonHandler() = default;
    virtual void onDoubleClick(void (*callback)()) = 0;
    virtual void update() = 0;
};

#endif
