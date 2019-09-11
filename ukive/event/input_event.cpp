#include "ukive/event/input_event.h"

#include "ukive/log.h"


namespace ukive {

    InputEvent::InputEvent()
        : mouse_wheel_(0),
          mouse_key_(0),
          cur_touch_id_(-1),
          char_key_(0),
          virtual_key_(0),
          event_type_(EV_NONE),
          pointer_type_(PT_NONE),
          is_outside_(false),
          is_no_dispatch_(false) { }

    InputEvent::~InputEvent() {
    }

    void InputEvent::setEvent(int ev) {
        event_type_ = ev;
    }

    void InputEvent::setPointerType(int type) {
        pointer_type_ = type;
    }

    void InputEvent::setX(int x) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.x = x;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].x = x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setY(int y) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.y = y;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].y = y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setX(int x, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].x = x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setY(int y, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].y = y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawX(int raw_x) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw_x = raw_x;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw_x = raw_x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawY(int raw_y) {
        if (pointer_type_ == PT_MOUSE) {
            mouse_pos_.raw_y = raw_y;
            return;
        }
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[cur_touch_id_].raw_y = raw_y;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawX(int raw_x, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw_x = raw_x;
            return;
        }
        DCHECK(false);
    }

    void InputEvent::setRawY(int raw_y, int id) {
        if (pointer_type_ == PT_TOUCH) {
            touch_pos_[id].raw_y = raw_y;
            return;
        }
        DCHECK(false);
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

    void InputEvent::setCurTouchId(int id) {
        cur_touch_id_ = id;
    }

    void InputEvent::setOutside(bool outside) {
        is_outside_ = outside;
    }

    void InputEvent::setIsNoDispatch(bool captured) {
        is_no_dispatch_ = captured;
    }

    void InputEvent::offsetInputPos(int dx, int dy) {
        if (event_type_ == EV_CANCEL ||
            event_type_ == EV_LEAVE_VIEW)
        {
            return;
        }

        if (isMouseEvent()) {
            mouse_pos_.x += dx;
            mouse_pos_.y += dy;
        } else if (isTouchEvent()) {
            for (auto& pos : touch_pos_) {
                pos.second.x += dx;
                pos.second.y += dy;
            }
        }
    }

    int InputEvent::getEvent() const {
        return event_type_;
    }

    int InputEvent::getPointerType() const {
        return pointer_type_;
    }

    int InputEvent::getX() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.x;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getY() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.y;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getX(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getY(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawX() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw_x;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw_x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawY() const {
        if (pointer_type_ == PT_MOUSE) {
            return mouse_pos_.raw_y;
        }
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                return it->second.raw_y;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawX(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw_x;
            }
        }
        DCHECK(false);
        return 0;
    }

    int InputEvent::getRawY(int id) const {
        if (pointer_type_ == PT_TOUCH) {
            auto it = touch_pos_.find(id);
            if (it != touch_pos_.end()) {
                return it->second.raw_y;
            }
        }
        DCHECK(false);
        return 0;
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

    int InputEvent::getCurTouchId() const {
        return cur_touch_id_;
    }

    bool InputEvent::isMouseEvent() const {
        DCHECK(isNoActiveEvent() || pointer_type_ != PT_NONE);
        return pointer_type_ == PT_MOUSE;
    }

    bool InputEvent::isTouchEvent() const {
        DCHECK(isNoActiveEvent() || pointer_type_ != PT_NONE);
        return pointer_type_ == PT_TOUCH;
    }

    bool InputEvent::isKeyboardEvent() const {
        DCHECK(isNoActiveEvent() || pointer_type_ != PT_NONE);
        return pointer_type_ == PT_KEYBOARD;
    }

    bool InputEvent::isNoDispatch() const {
        return is_no_dispatch_;
    }

    bool InputEvent::isNoActiveEvent() const {
        return event_type_ == EV_CANCEL || event_type_ == EV_LEAVE_VIEW;
    }

    bool InputEvent::isOutside() const {
        return is_outside_;
    }

    void InputEvent::combineTouchEvent(InputEvent* e) {
        if (!e->isTouchEvent()) {
            DCHECK(false);
            return;
        }

        if (event_type_ == EV_NONE) {
            *this = *e;
        } else if (isTouchEvent()) {
            switch (e->getEvent()) {
            case EVT_DOWN:
                DCHECK(touch_pos_.find(e->getCurTouchId()) == touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getX(), e->getY(), e->getRawX(), e->getRawY() };
                event_type_ = touch_pos_.size() > 1 ? EVT_MULTI_DOWN : EVT_DOWN;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EVT_UP:
                DCHECK(touch_pos_.find(e->getCurTouchId()) != touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getX(), e->getY(), e->getRawX(), e->getRawY() };
                event_type_ = touch_pos_.size() > 1 ? EVT_MULTI_UP : EVT_UP;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EVT_MOVE:
                DCHECK(touch_pos_.find(e->getCurTouchId()) != touch_pos_.end());
                touch_pos_[e->getCurTouchId()] = { e->getX(), e->getY(), e->getRawX(), e->getRawY() };
                event_type_ = EVT_MOVE;
                cur_touch_id_ = e->getCurTouchId();
                break;

            case EV_LEAVE_VIEW:
            case EV_CANCEL:
                event_type_ = e->getEvent();
                break;

            default:
                DCHECK(false);
                break;
            }
        } else {
            DCHECK(false);
        }
    }

    void InputEvent::clearTouchUp() {
        if (!isTouchEvent()) {
            DCHECK(false);
            return;
        }

        if (event_type_ == EVT_UP ||
            event_type_ == EVT_MULTI_UP)
        {
            auto it = touch_pos_.find(cur_touch_id_);
            if (it != touch_pos_.end()) {
                touch_pos_.erase(it);
            } else {
                DCHECK(false);
            }
        } else if (event_type_ == EV_LEAVE_VIEW ||
            event_type_ == EV_CANCEL)
        {
            touch_pos_.clear();
            cur_touch_id_ = -1;
        }
    }

    void InputEvent::clearTouch() {
        touch_pos_.clear();
        cur_touch_id_ = -1;
    }

}
