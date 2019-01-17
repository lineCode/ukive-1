#ifndef UKIVE_EVENT_INPUT_EVENT_H_
#define UKIVE_EVENT_INPUT_EVENT_H_

#include <map>


namespace ukive {

    class InputEvent {
    public:
        // 鼠标事件
        enum {
            EVM_DOWN = 1,
            EVM_UP,
            EVM_MOVE,
            EVM_WHEEL,
            EVM_LEAVE_WIN,
            EVM_LEAVE_VIEW,
            EVM_HOVER,
            EVM_SCROLL_ENTER,
        };

        // 触摸事件
        enum {
            EVT_DOWN = (EVM_SCROLL_ENTER + 1),
            EVT_MULTI_DOWN,
            EVT_MULTI_UP,
            EVT_UP,
            EVT_MOVE
        };

        // 键盘事件
        enum {
            EVK_DOWN = (EVT_MOVE + 1),
            EVK_UP,
            EVK_CHAR,
        };

        // 其他事件
        enum {
            EV_CANCEL = (EVK_CHAR + 1),
        };

        // 鼠标按键定义
        enum {
            MK_LEFT = 0,
            MK_MIDDLE,
            MK_RIGHT,
        };

        // 键盘按键定义使用 Windows 本身的定义。

    public:
        InputEvent();
        InputEvent(const InputEvent& source) = default;
        ~InputEvent();

        void setEvent(int ev);
        void setMouseX(int x);
        void setMouseY(int y);
        void setMouseRawX(int raw_x);
        void setMouseRawY(int raw_y);
        void setMouseWheel(int wheel);
        void setMouseKey(int key);
        void setKeyboardCharKey(int char_key, int ex_msg);
        void setKeyboardVirtualKey(int virtual_key, int ex_msg);
        void setTouchX(int x, int id);
        void setTouchY(int y, int id);
        void setTouchRawX(int raw_x, int id);
        void setTouchRawY(int raw_y, int id);
        void setCurTouchId(int id);

        void setOutside(bool outside);
        void setIsMouseCaptured(bool captured);

        int getEvent() const;
        int getMouseX() const;
        int getMouseY() const;
        int getMouseRawX() const;
        int getMouseRawY() const;
        int getMouseWheel() const;
        int getMouseKey() const;
        int getKeyboardCharKey() const;
        int getKeyboardVirtualKey() const;
        int getTouchX(int id) const;
        int getTouchY(int id) const;
        int getTouchRawX(int id) const;
        int getTouchRawY(int id) const;
        int getCurTouchId() const;

        bool isMouseEvent() const;
        bool isKeyboardEvent() const;
        bool isMouseCaptured() const;

        /**
         * 当鼠标事件发生于Widget外部时，该方法返回true。
         * 只有当 View 的 setReceiveOutsideInputEvent() 方法以 true 为参数
         * 调用之后，此方法才有效。
         */
        bool isOutside() const;

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

        bool is_outside_;
        bool is_mouse_captured_;
    };

}

#endif  // UKIVE_EVENT_INPUT_EVENT_H_