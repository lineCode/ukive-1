#ifndef UKIVE_EVENT_INPUT_EVENT_H_
#define UKIVE_EVENT_INPUT_EVENT_H_

#include <map>


namespace ukive {

    class InputEvent {
    public:
        enum {
            EV_NONE = 0,
            EV_LEAVE_VIEW,
            EV_CANCEL,
        };

        // 鼠标事件
        enum {
            EVM_DOWN = EV_CANCEL + 1,
            EVM_UP,
            EVM_MOVE,
            EVM_WHEEL,
            EVM_LEAVE_WIN,
            EVM_HOVER,
            EVM_SCROLL_ENTER,
        };

        // 触摸事件
        enum {
            EVT_DOWN = (EVM_SCROLL_ENTER + 1),
            EVT_MULTI_DOWN,
            EVT_MULTI_UP,
            EVT_UP,
            EVT_MOVE,
        };

        // 键盘事件
        enum {
            EVK_DOWN = (EVT_MOVE + 1),
            EVK_UP,
            EVK_CHAR,
        };

        // 鼠标按键定义
        enum {
            MK_LEFT = 0,
            MK_MIDDLE,
            MK_RIGHT,
        };

        // 键盘按键定义使用 Windows 本身的定义。

        enum PointerType {
            PT_NONE,
            PT_MOUSE,
            PT_TOUCH,
            PT_PEN,
        };

    public:
        InputEvent();
        InputEvent(const InputEvent& source) = default;
        ~InputEvent();

        void setEvent(int ev);
        void setPointerType(int type);
        void setX(int x);
        void setY(int y);
        void setX(int x, int id);
        void setY(int y, int id);
        void setRawX(int raw_x);
        void setRawY(int raw_y);
        void setRawX(int raw_x, int id);
        void setRawY(int raw_y, int id);
        void setMouseWheel(int wheel);
        void setMouseKey(int key);
        void setKeyboardCharKey(int char_key, int ex_msg);
        void setKeyboardVirtualKey(int virtual_key, int ex_msg);
        void setCurTouchId(int id);
        void setOutside(bool outside);
        void setIsNoDispatch(bool captured);
        void offsetInputPos(int dx, int dy);

        int getEvent() const;
        int getPointerType() const;
        int getX() const;
        int getY() const;
        int getX(int id) const;
        int getY(int id) const;
        int getRawX() const;
        int getRawY() const;
        int getRawX(int id) const;
        int getRawY(int id) const;
        int getMouseWheel() const;
        int getMouseKey() const;
        int getKeyboardCharKey() const;
        int getKeyboardVirtualKey() const;
        int getCurTouchId() const;

        bool isMouseEvent() const;
        bool isTouchEvent() const;
        bool isKeyboardEvent() const;
        bool isNoDispatch() const;

        /**
         * 当鼠标事件发生于 View 外部时，该方法返回 true。
         * 只有当 View 的 setReceiveOutsideInputEvent() 方法以 true 为参数
         * 调用之后，此方法才有效。
         */
        bool isOutside() const;

        void combineTouchEvent(InputEvent* e);
        void clearTouchUp();
        void clearTouch();

    private:
        struct InputPos {
            int x = 0;
            int y = 0;
            int raw_x = 0;
            int raw_y = 0;
        };

        int mouse_wheel_;
        int mouse_key_;
        InputPos mouse_pos_;
        std::map<int, InputPos> touch_pos_;
        int cur_touch_id_;

        int char_key_;
        int virtual_key_;

        int event_type_;
        int pointer_type_;

        bool is_outside_;
        bool is_no_dispatch_;
    };

}

#endif  // UKIVE_EVENT_INPUT_EVENT_H_