#ifndef UKIVE_WINDOW_WINDOW_H_
#define UKIVE_WINDOW_WINDOW_H_

#include <Windows.h>

#include <memory>

#include "ukive/message/cycler.h"
#include "ukive/utils/string_utils.h"
#include "ukive/graphics/color.h"
#include "ukive/animation/animation_manager.h"
#include "ukive/views/view.h"
#include "ukive/utils/weak_bind.h"
#include "ukive/graphics/swapchain_resize_notifier.h"


namespace ukive {

    class Rect;
    class Cycler;
    class Canvas;
    class Renderer;
    class InputEvent;
    class WindowImpl;
    class RootLayout;
    class AnimationManager;
    class ContextMenu;
    class ContextMenuCallback;
    class TextActionMode;
    class TextActionModeCallback;
    struct ClassInfo;

    class Window : public SwapChainResizeNotifier {
    public:
        Window();
        virtual ~Window();

        void show();
        void hide();
        void focus();
        void close();
        void center();

        void setTitle(const string16& title);
        void setX(int x);
        void setY(int y);
        void setPosition(int x, int y);
        void setWidth(int width);
        void setHeight(int height);
        void setBounds(int x, int y, int width, int height);
        void setMinWidth(int min_width);
        void setMinHeight(int min_height);
        void setCurrentCursor(Cursor cursor);
        void setContentView(View* content);
        void setBackgroundColor(Color color);
        void setTranslucent(bool translucent);
        void setStartupWindow(bool enable);

        int getX();
        int getY();
        int getWidth();
        int getHeight();
        int getMinWidth();
        int getMinHeight();
        int getClientWidth();
        int getClientHeight();
        RootLayout* getRootLayout();
        Color getBackgroundColor();
        Cycler* getCycler();
        Renderer* getRenderer();
        HWND getHandle();
        AnimationManager* getAnimationManager();

        bool isShowing();
        bool isCursorInClient();
        bool isTranslucent();
        bool isStartupWindow();

        void captureMouse(View* v);
        void releaseMouse();

        //当一个widget获取到焦点时，应调用此方法。
        void captureKeyboard(View* v);
        //当一个widget放弃焦点时，应调用此方法。
        void releaseKeyboard();

        View* getMouseHolder();
        unsigned int getMouseHolderRef();
        View* getKeyboardHolder();

        void invalidate();
        void invalidate(int left, int top, int right, int bottom);
        void requestLayout();

        void performLayout();
        void performRefresh();
        void performRefresh(int left, int top, int right, int bottom);

        View* findViewById(int id);

        ContextMenu* startContextMenu(
            ContextMenuCallback* callback, View* anchor, View::Gravity gravity);
        TextActionMode* startTextActionMode(TextActionModeCallback* callback);

        float dpToPx(float dp);
        float pxToDp(int px);

        virtual void onPreCreate(
            ClassInfo* info,
            int* win_style, int* win_ex_style);
        virtual void onCreate();
        virtual void onShow(bool show);
        virtual void onActivate(int param);
        virtual void onSetFocus();
        virtual void onKillFocus();
        virtual void onDraw(const Rect& rect);
        virtual void onMove(int x, int y);
        virtual void onResize(
            int param, int width, int height,
            int client_width, int client_height);
        virtual bool onMoving(Rect* rect);
        virtual bool onResizing(int edge, Rect* rect);
        virtual bool onClose();
        virtual void onDestroy();
        virtual bool onInputEvent(InputEvent* e);
        virtual void onDpiChanged(int dpi_x, int dpi_y);
        virtual bool onDataCopy(unsigned int id, unsigned int size, void* data);

        virtual void onDrawCanvas(Canvas* canvas);

    protected:
        void onPreSwapChainResize() override;
        void onPostSwapChainResize() override;

    private:
        enum {
            SCHEDULE_RENDER = 0,
            SCHEDULE_LAYOUT = 1,
        };

        class UpdateCycler : public Cycler {
        public:
            UpdateCycler(Window* window)
                :win_(window) {}

            void handleMessage(Message* msg) override;
        private:
            Window* win_;
        };

        class AnimStateChangedListener
            : public AnimationManager::OnStateChangedListener {
        public:
            AnimStateChangedListener(Window* window)
                :win_(window) {}

            void onStateChanged(
                UI_ANIMATION_MANAGER_STATUS newStatus,
                UI_ANIMATION_MANAGER_STATUS previousStatus) override;
        private:
            Window* win_;
        };

        class AnimTimerEventListener
            : public AnimationManager::OnTimerEventListener {
        public:
            AnimTimerEventListener(Window* window)
                :window_(window) {}

            void onPreUpdate() override;
            void onPostUpdate() override;
            void onRenderingTooSlow(unsigned int fps) override;

        private:
            Window* window_;
        };


        std::unique_ptr<WindowImpl> impl_;

        Canvas* canvas_;
        Renderer* renderer_;

        Cycler* labour_cycler_;
        RootLayout* root_layout_;

        View* mouse_holder_;
        View* focus_holder_;
        View* focus_holder_backup_;
        unsigned int mouse_holder_ref_;

        std::shared_ptr<ContextMenu> context_menu_;
        std::shared_ptr<TextActionMode> text_action_mode_;

        AnimationManager* anim_mgr_;
        AnimationManager::OnStateChangedListener* mStateChangedListener;

        AnimStateChangedListener* mAnimStateChangedListener;
        AnimTimerEventListener* mAnimTimerEventListener;

        Color background_color_;
        bool is_startup_window_;
        int min_width_, min_height_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_H_