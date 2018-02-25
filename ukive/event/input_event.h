#ifndef UKIVE_EVENT_INPUT_EVENT_H_
#define UKIVE_EVENT_INPUT_EVENT_H_


namespace ukive {

    class InputEvent
    {
    public:
        // 鼠标事件。
        static const int EVM_DOWN = 1;
        static const int EVM_UP = 2;
        static const int EVM_MOVE = 3;
        static const int EVM_WHEEL = 4;
        static const int EVM_LEAVE_WIN = 5;
        static const int EVM_LEAVE_VIEW = 6;
        static const int EVM_HOVER = 7;
        static const int EVM_SCROLL_ENTER = 8;

        // 键盘事件。
        static const int EVK_DOWN = 100;
        static const int EVK_UP = 101;

        // 其他事件。
        static const int EV_CANCEL = 400;

        // 鼠标按键定义。
        static const int MK_LEFT = 100;
        static const int MK_MIDDLE = 101;
        static const int MK_RIGHT = 102;

        // 键盘按键定义使用windows本身的定义。

    public:
        InputEvent();
        InputEvent(const InputEvent &source);
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
        /// 只有当Widget的setReceiveOutsideInputEvent()方法以true为参数
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