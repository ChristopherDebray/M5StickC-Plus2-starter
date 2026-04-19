#ifndef BUTTON_HANDLER_M5STICK_ADAPTER_H
#define BUTTON_HANDLER_M5STICK_ADAPTER_H

#include <M5Unified.h>
#include <Arduino.h>
#include "../ports/button_handler_port.h"

class ButtonHandlerM5StickAdapter : public IButtonHandler {
public:
    ButtonHandlerM5StickAdapter(int btnType = 1, uint32_t doubleClickMs = 400)
        : _lastPressTime(0), _waitingSecondPress(false),
          _doublePressDuration(doubleClickMs), _onDoubleClickCallback(nullptr) {
        if      (btnType == 0) _buttonType = BTN_A;
        else if (btnType == 1) _buttonType = BTN_B;
        else                   _buttonType = BTN_C;
    }

    void onDoubleClick(void (*callback)()) override {
        _onDoubleClickCallback = callback;
    }

    void update() override {
        if (isButtonPressed()) {
            uint32_t now = millis();
            if (!_waitingSecondPress) {
                _waitingSecondPress = true;
                _lastPressTime = now;
            } else {
                if (now - _lastPressTime <= _doublePressDuration) {
                    _waitingSecondPress = false;
                    if (_onDoubleClickCallback) _onDoubleClickCallback();
                } else {
                    _lastPressTime = now;
                }
            }
        }
        if (_waitingSecondPress && (millis() - _lastPressTime > _doublePressDuration)) {
            _waitingSecondPress = false;
        }
    }

private:
    enum ButtonType { BTN_A, BTN_B, BTN_C } _buttonType;
    uint32_t  _lastPressTime;
    bool      _waitingSecondPress;
    uint32_t  _doublePressDuration;
    void    (*_onDoubleClickCallback)();

    bool isButtonPressed() {
        switch (_buttonType) {
            case BTN_A: return M5.BtnA.wasPressed();
            case BTN_B: return M5.BtnB.wasPressed();
            case BTN_C: return M5.BtnC.wasPressed();
            default:    return false;
        }
    }
};

#endif
