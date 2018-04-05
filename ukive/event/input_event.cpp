#include "input_event.h"


namespace ukive {

    InputEvent::InputEvent()
        :is_outside_(false),
        is_mouse_captured_(false) {
    }

    InputEvent::~InputEvent() {
    }


    void InputEvent::setEvent(int ev) {
        event_type_ = ev;
    }

    void InputEvent::setMouseX(int x) {
        mouse_x_ = x;
    }

    void InputEvent::setMouseY(int y) {
        mouse_y_ = y;
    }

    void InputEvent::setMouseRawX(int raw_x) {
        mouse_raw_x_ = raw_x;
    }

    void InputEvent::setMouseRawY(int raw_y) {
        mouse_raw_y_ = raw_y;
    }

    void InputEvent::setMouseWheel(int wheel) {
        mouse_wheel_ = wheel;
    }

    void InputEvent::setMouseKey(int key) {
        mouse_key_ = key;
    }

    void InputEvent::setKeyboardKey(int virtual_key, int ex_msg) {
        int curSlot = ex_msg;
        //For WM_KEYDOWN:
        //The repeat count for the current message.
        //The value is the number of times the keystroke is autorepeated as a result of the user holding down the key.
        //If the keystroke is held long enough, multiple messages are sent. However, the repeat count is not cumulative.
        //
        //For WM_KEYUP:
        //The repeat count for the current message.
        //The value is the number of times the keystroke is autorepeated as a result of the user holding down the key.
        //The repeat count is always 1 for a WM_KEYUP message.
        int repeatCount = curSlot & 0x0000ffff;  //0-15bit

        curSlot = (curSlot & 0xffff0000) >> 16;
        //The scan code. The value depends on the OEM.
        int scanCode = curSlot & 0x00ff;         //16-23bit

        curSlot = (curSlot & 0xff00) >> 8;
        //Indicates whether the key is an extended key,
        //such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard.
        //The value is 1 if it is an extended key; otherwise, it is 0.
        int isExtendKey = curSlot & 0x01;        //24bit

        curSlot = (curSlot & 0xfe) >> 1;
        //Reserved; do not use.
        int reserved = curSlot & 0x0f;           //25-28bit

        curSlot = (curSlot & 0xf0) >> 4;
        //The context code. The value is always 0 for a WM_KEYDOWN message.
        //The context code. The value is always 0 for a WM_KEYUP message.
        int contextCode = curSlot & 0x01;        //29bit

        curSlot = (curSlot & 0xfe) >> 1;
        //The previous key state.
        //The value is 1 if the key is down before the message is sent, or it is zero if the key is up.
        //The previous key state. The value is always 1 for a WM_KEYUP message.
        int prevKeyState = curSlot & 0x01;       //30bit

        curSlot = (curSlot & 0xfe) >> 1;
        //The transition state. The value is always 0 for a WM_KEYDOWN message.
        //The transition state. The value is always 1 for a WM_KEYUP message.
        int transitionState = curSlot;           //31bit

        virtual_key_ = virtual_key;
    }

    void InputEvent::setOutside(bool outside) {
        is_outside_ = outside;
    }

    void InputEvent::setIsMouseCaptured(bool captured) {
        is_mouse_captured_ = captured;
    }


    int InputEvent::getEvent() {
        return event_type_;
    }

    int InputEvent::getMouseX() {
        return mouse_x_;
    }

    int InputEvent::getMouseY() {
        return mouse_y_;
    }

    int InputEvent::getMouseRawX() {
        return mouse_raw_x_;
    }

    int InputEvent::getMouseRawY() {
        return mouse_raw_y_;
    }

    int InputEvent::getMouseWheel() {
        return mouse_wheel_;
    }

    int InputEvent::getMouseKey() {
        return mouse_key_;
    }

    int InputEvent::getKeyboardKey() {
        return virtual_key_;
    }


    bool InputEvent::isMouseEvent() {
        return (event_type_ == EVM_DOWN
            || event_type_ == EVM_UP
            || event_type_ == EVM_MOVE
            || event_type_ == EVM_WHEEL
            || event_type_ == EVM_LEAVE_WIN
            || event_type_ == EVM_LEAVE_VIEW
            || event_type_ == EVM_SCROLL_ENTER
            || event_type_ == EVM_HOVER);
    }

    bool InputEvent::isKeyboardEvent() {
        return (event_type_ == EVK_DOWN
            || event_type_ == EVK_UP);
    }

    bool InputEvent::isMouseCaptured() {
        return is_mouse_captured_;
    }

    bool InputEvent::isOutside() {
        return is_outside_;
    }

}