#ifndef UKIVE_EVENT_INPUT_EVENT_H_
#define UKIVE_EVENT_INPUT_EVENT_H_


namespace ukive {

    class InputEvent
    {
    public:
        // 鼠标事件。
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

        // 键盘事件。
        enum {
            EVK_DOWN = (EVM_SCROLL_ENTER + 1),
            EVK_UP,
        };

        // 其他事件。
        enum {
            EV_CANCEL = (EVK_UP + 1),
        };

        // 鼠标按键定义。
        enum {
            MK_LEFT = 0,
            MK_MIDDLE,
            MK_RIGHT,
        };

        // 键盘按键定义使用windows本身的定义。

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
        void setKeyboardKey(int virtual_key, int ex_msg);

        void setOutside(bool outside);
        void setIsMouseCaptured(bool captured);

        int getEvent();
        int getMouseX();
        int getMouseY();
        int getMouseRawX();
        int getMouseRawY();
        int getMouseWheel();
        int getMouseKey();
        int getKeyboardKey();

        bool isMouseEvent();
        bool isKeyboardEvent();
        bool isMouseCaptured();

        /// <summary>
        /// 当鼠标事件发生于Widget外部时，该方法返回true。
        /// 只有当 View 的 setReceiveOutsideInputEvent() 方法以 true 为参数
        /// 调用之后，此方法才有效。
        /// </summary>
        bool isOutside();

    private:
        int mouse_x_;
        int mouse_y_;
        int mouse_raw_x_;
        int mouse_raw_y_;
        int mouse_wheel_;
        int mouse_key_;

        int virtual_key_;

        int event_type_;

        bool is_outside_;
        bool is_mouse_captured_;
    };

}

#endif  // UKIVE_EVENT_INPUT_EVENT_H_