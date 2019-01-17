#include "input_event.h"


namespace ukive {

    InputEvent::InputEvent()
        : mouse_wheel_(0),
          mouse_key_(0),
          cur_touch_id_(-1),
          char_key_(0),
          virtual_key_(0),
          event_type_(0),
          is_outside_(false),
          is_mouse_captured_(false) {
    }

    InputEvent::~InputEvent() {
    }

    void InputEvent::setEvent(int ev) {
        event_type_ = ev;
    }

    void InputEvent::setMouseX(int x) {
        mouse_pos_.x = x;
    }

    void InputEvent::setMouseY(int y) {
        mouse_pos_.y = y;
    }

    void InputEvent::setMouseRawX(int raw_x) {
        mouse_pos_.raw_x = raw_x;
    }

    void InputEvent::setMouseRawY(int raw_y) {
        mouse_pos_.raw_y = raw_y;
    }

    void InputEvent::setMouseWheel(int wheel) {
        mouse_wheel_ = wheel;
    }

    void InputEvent::setMouseKey(int key) {
        mouse_key_ = key;
    }

    void InputEvent::setKeyboardCharKey(int char_key, int ex_msg) {
        int cur_slot = ex_msg;
        // For WM_KEYDOWN:
        // The repeat count for the current message.
        // The value is the number of times the keystroke is autorepeated as a result of the user holding down the key.
        // If the keystroke is held long enough, multiple messages are sent. However, the repeat count is not cumulative.
        //
        // For WM_KEYUP:
        // The repeat count for the current message.
        // The value is the number of times the keystroke is autorepeated as a result of the user holding down the key.
        // The repeat count is always 1 for a WM_KEYUP message.
        int repeatCount = cur_slot & 0x0000ffff;  //0-15bit

        cur_slot = (cur_slot & 0xffff0000) >> 16;
        // The scan code. The value depends on the OEM.
        int scanCode = cur_slot & 0x00ff;         //16-23bit

        cur_slot = (cur_slot & 0xff00) >> 8;
        // Indicates whether the key is an extended key,
        // such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard.
        // The value is 1 if it is an extended key; otherwise, it is 0.
        int isExtendKey = cur_slot & 0x01;        //24bit

        cur_slot = (cur_slot & 0xfe) >> 1;
        // Reserved; do not use.
        int reserved = cur_slot & 0x0f;           //25-28bit

        cur_slot = (cur_slot & 0xf0) >> 4;
        // The context code. The value is always 0 for a WM_KEYDOWN message.
        // The context code. The value is always 0 for a WM_KEYUP message.
        int contextCode = cur_slot & 0x01;        //29bit

        cur_slot = (cur_slot & 0xfe) >> 1;
        // The previous key state.
        // The value is 1 if the key is down before the message is sent, or it is zero if the key is up.
        // The previous key state. The value is always 1 for a WM_KEYUP message.
        int prevKeyState = cur_slot & 0x01;       //30bit

        cur_slot = (cur_slot & 0xfe) >> 1;
        // The transition state. The value is always 0 for a WM_KEYDOWN message.
        // The transition state. The value is always 1 for a WM_KEYUP message.
        int transitionState = cur_slot;           //31bit

        char_key_ = char_key;
    }

    void InputEvent::setKeyboardVirtualKey(int virtual_key, int ex_msg) {
        int cur_slot = ex_msg;
        // For WM_KEYDOWN:
        // The repeat count for the current message.
        // The value is the number of times the keystroke is autorepeated as a result of the user holding down the key.
        // If the keystroke is held long enough, multiple messages are sent. However, the repeat count is not cumulative.
        //
        // For WM_KEYUP:
        // The repeat count for the current message.
        // The value is the number of times the keystroke is autorepeated as a result of the user holding down the key.
        // The repeat count is always 1 for a WM_KEYUP message.
        int repeatCount = cur_slot & 0x0000ffff;  //0-15bit

        cur_slot = (cur_slot & 0xffff0000) >> 16;
        // The scan code. The value depends on the OEM.
        int scanCode = cur_slot & 0x00ff;         //16-23bit

        cur_slot = (cur_slot & 0xff00) >> 8;
        // Indicates whether the key is an extended key,
        // such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard.
        // The value is 1 if it is an extended key; otherwise, it is 0.
        int isExtendKey = cur_slot & 0x01;        //24bit

        cur_slot = (cur_slot & 0xfe) >> 1;
        // Reserved; do not use.
        int reserved = cur_slot & 0x0f;           //25-28bit

        cur_slot = (cur_slot & 0xf0) >> 4;
        // The context code. The value is always 0 for a WM_KEYDOWN message.
        // The context code. The value is always 0 for a WM_KEYUP message.
        int contextCode = cur_slot & 0x01;        //29bit

        cur_slot = (cur_slot & 0xfe) >> 1;
        // The previous key state.
        // The value is 1 if the key is down before the message is sent, or it is zero if the key is up.
        // The previous key state. The value is always 1 for a WM_KEYUP message.
        int prevKeyState = cur_slot & 0x01;       //30bit

        cur_slot = (cur_slot & 0xfe) >> 1;
        // The transition state. The value is always 0 for a WM_KEYDOWN message.
        // The transition state. The value is always 1 for a WM_KEYUP message.
        int transitionState = cur_slot;           //31bit

        virtual_key_ = virtual_key;
    }

    void InputEvent::setTouchX(int x, int id) {
        touch_pos_[id].x = x;
    }

    void InputEvent::setTouchY(int y, int id) {
        touch_pos_[id].y = y;
    }

    void InputEvent::setTouchRawX(int raw_x, int id) {
        touch_pos_[id].raw_x = raw_x;
    }

    void InputEvent::setTouchRawY(int raw_y, int id) {
        touch_pos_[id].raw_y = raw_y;
    }

    void InputEvent::setCurTouchId(int id) {
        cur_touch_id_ = id;
    }

    void InputEvent::setOutside(bool outside) {
        is_outside_ = outside;
    }

    void InputEvent::setIsMouseCaptured(bool captured) {
        is_mouse_captured_ = captured;
    }

    int InputEvent::getEvent() const {
        return event_type_;
    }

    int InputEvent::getMouseX() const {
        return mouse_pos_.x;
    }

    int InputEvent::getMouseY() const {
        return mouse_pos_.y;
    }

    int InputEvent::getMouseRawX() const {
        return mouse_pos_.raw_x;
    }

    int InputEvent::getMouseRawY() const {
        return mouse_pos_.raw_y;
    }

    int InputEvent::getMouseWheel() const {
        return mouse_wheel_;
    }

    int InputEvent::getMouseKey() const {
        return mouse_key_;
    }

    int InputEvent::getKeyboardCharKey() const {
        return char_key_;
    }

    int InputEvent::getKeyboardVirtualKey() const {
        return virtual_key_;
    }

    int InputEvent::getTouchX(int id) const {
        auto it = touch_pos_.find(id);
        if (it != touch_pos_.end()) {
            return it->second.x;
        }
        return 0;
    }

    int InputEvent::getTouchY(int id) const {
        auto it = touch_pos_.find(id);
        if (it != touch_pos_.end()) {
            return it->second.y;
        }
        return 0;
    }

    int InputEvent::getTouchRawX(int id) const {
        auto it = touch_pos_.find(id);
        if (it != touch_pos_.end()) {
            return it->second.raw_x;
        }
        return 0;
    }

    int InputEvent::getTouchRawY(int id) const {
        auto it = touch_pos_.find(id);
        if (it != touch_pos_.end()) {
            return it->second.raw_y;
        }
        return 0;
    }

    int InputEvent::getCurTouchId() const {
        return cur_touch_id_;
    }

    bool InputEvent::isMouseEvent() const {
        return (event_type_ >= EVM_DOWN
            && event_type_ <= EVM_SCROLL_ENTER);
    }

    bool InputEvent::isKeyboardEvent() const {
        return (event_type_ >= EVK_DOWN
            && event_type_ <= EVK_CHAR);
    }

    bool InputEvent::isMouseCaptured() const {
        return is_mouse_captured_;
    }

    bool InputEvent::isOutside() const {
        return is_outside_;
    }

}