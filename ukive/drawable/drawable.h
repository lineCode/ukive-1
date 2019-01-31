#ifndef UKIVE_DRAWABLE_DRAWABLE_H_
#define UKIVE_DRAWABLE_DRAWABLE_H_

#include "ukive/graphics/rect.h"


namespace ukive {

    class Canvas;
    class Drawable;

    class DrawableCallback {
    public:
        virtual ~DrawableCallback() = default;

        virtual void onDrawableInvalidate(Drawable* d) = 0;
    };

    class Drawable {
    public:
        enum DrawableState {
            STATE_NONE,
            STATE_PRESSED,
            STATE_FOCUSED,
            STATE_HOVERED,
            STATE_DISABLED,
        };

        Drawable();
        virtual ~Drawable() = default;

        void setBounds(const Rect& rect);
        void setBounds(int left, int top, int width, int height);
        void setCallback(DrawableCallback* callback);

        bool setState(int state);
        void setHotspot(int x, int y);
        bool setParentFocus(bool focus);

        int getState() const;
        int getPrevState() const;
        Rect getBounds() const;

        virtual void draw(Canvas* canvas) = 0;

        virtual float getOpacity() const;

        virtual int getIncWidth() const;
        virtual int getIncHeight() const;

    protected:
        void invalidate();

        virtual bool onFocusChanged(bool focus);
        virtual void onBoundChanged(const Rect& new_bound);
        virtual bool onStateChanged(int new_state, int prev_state);

        int start_x_, start_y_;

    private:
        int state_;
        int prev_state_;
        bool is_parent_has_focus_;
        Rect bounds_;
        DrawableCallback* callback_;
    };

}

#endif  // UKIVE_DRAWABLE_DRAWABLE_H_