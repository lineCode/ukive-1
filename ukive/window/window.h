#ifndef UKIVE_WINDOW_WINDOW_H_
#define UKIVE_WINDOW_WINDOW_H_

#include <Windows.h>

#include <memory>
#include <vector>

#include "ukive/message/cycler.h"
#include "ukive/utils/string_utils.h"
#include "ukive/graphics/color.h"
#include "ukive/animation/animation_manager.h"
#include "ukive/views/view.h"
#include "ukive/graphics/swapchain_resize_notifier.h"


namespace ukive {

    class Rect;
    class Cycler;
    class Canvas;
    class Renderer;
    class InputEvent;
    class WindowImpl;
    class TitleBar;
    class RootLayout;
    class AnimationManager;
    class ContextMenu;
    class ContextMenuCallback;
    class TextActionMode;
    class TextActionModeCallback;
    class OnWindowStatusChangedListener;
    struct ClassInfo;


    class Window : public SwapChainResizeNotifier, public CyclerListener {
    public:
        enum FrameType {
            FRAME_NATIVE,
            FRAME_CUSTOM,
        };

        Window();
        virtual ~Window();

        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();
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
        void setContentView(int layout_id);
        void setContentView(View* content);
        void setBackgroundColor(Color color);
        void setTranslucent(bool translucent);
        void setStartupWindow(bool enable);
        void setFrameType(FrameType type);
        void setLastInputView(View* v);

        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        int getMinWidth() const;
        int getMinHeight() const;
        int getClientWidth() const;
        int getClientHeight() const;
        string16 getTitle() const;
        RootLayout* getRootLayout() const;
        Color getBackgroundColor() const;
        Cycler* getCycler() const;
        Renderer* getRenderer() const;
        HWND getHandle() const;
        AnimationManager* getAnimationManager() const;
        FrameType getFrameType() const;
        View* getLastInputView() const;
        TitleBar* getTitleBar() const;

        bool isShowing() const;
        bool isTranslucent() const;
        bool isStartupWindow() const;
        bool isMinimum() const;
        bool isMaximum() const;
        bool isTitleBarShowing() const;

        void showTitleBar();
        void hideTitleBar();
        void removeTitleBar();

        void addStatusChangedListener(OnWindowStatusChangedListener* l);
        void removeStatusChangedListener(OnWindowStatusChangedListener* l);

        void convScreenToClient(Point* p);
        void convClientToScreen(Point* p);

        void captureMouse(View* v);
        void releaseMouse(bool all = false);

        // 当一个 View 获取到焦点时，应调用此方法。
        void captureKeyboard(View* v);
        // 当一个 View 放弃焦点时，应调用此方法。
        void releaseKeyboard();

        View* getMouseHolder() const;
        int getMouseHolderRef() const;
        View* getKeyboardHolder() const;

        void invalidate();
        void invalidate(int left, int top, int right, int bottom);
        void requestLayout();

        void performLayout();
        void performRefresh();
        void performRefresh(int left, int top, int right, int bottom);

        View* findViewById(int id) const;

        ContextMenu* startContextMenu(
            ContextMenuCallback* callback, View* anchor, View::Gravity gravity);
        void notifyContextMenuClose();
        TextActionMode* startTextActionMode(TextActionModeCallback* callback);
        void notifyTextActionModeClose();

        float dpToPx(float dp);
        float pxToDp(float px);

        virtual void onPreCreate(
            ClassInfo* info,
            int* win_style, int* win_ex_style);
        virtual void onCreate();
        virtual void onShow(bool show);
        virtual void onActivate(int param);
        virtual void onSetFocus();
        virtual void onKillFocus();
        virtual void onSetText(const string16& text);
        virtual void onSetIcon();
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
        virtual HitPoint onNCHitTest(int x, int y);
        virtual void onDpiChanged(int dpi_x, int dpi_y);
        virtual bool onDataCopy(unsigned int id, unsigned int size, void* data);

        virtual void onDrawCanvas(Canvas* canvas);

    protected:
        void onPreSwapChainResize() override;
        void onPostSwapChainResize() override;
        void onHandleMessage(Message* msg) override;

    private:
        enum {
            SCHEDULE_RENDER = 0,
            SCHEDULE_LAYOUT = 1,
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
        View* last_input_view_;
        int mouse_holder_ref_;

        ContextMenu* context_menu_;
        TextActionMode* text_action_mode_;

        AnimationManager* anim_mgr_;
        AnimationManager::OnStateChangedListener* mStateChangedListener;
        AnimStateChangedListener* mAnimStateChangedListener;
        AnimTimerEventListener* mAnimTimerEventListener;

        std::vector<OnWindowStatusChangedListener*> status_changed_listeners_;

        Color background_color_;
        bool is_startup_window_;
        int min_width_, min_height_;
        FrameType frame_type_;
    };

}

#endif  // UKIVE_WINDOW_WINDOW_H_