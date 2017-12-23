#ifndef UKIVE_DRAWABLE_DRAWABLE_H_
#define UKIVE_DRAWABLE_DRAWABLE_H_

#include "ukive/graphics/rect.h"


namespace ukive {

    class Canvas;

    class Drawable
    {
    public:
        enum DrawableState {
            STATE_NONE,
            STATE_PRESSED,
            STATE_FOCUSED,
            STATE_HOVERED
        };

        Drawable();
        virtual ~Drawable() = default;

        void setBounds(RectF &rect);
        void setBounds(int left, int top, int right, int bottom);

        bool setState(int state);
        void setHotspot(int x, int y);
        bool setParentFocus(bool focus);

        int getState();
        int getPrevState();
        RectF &getBounds();

        virtual void draw(Canvas *canvas) = 0;

        virtual float getOpacity();

        virtual int getIncWidth();
        virtual int getIncHeight();

    protected:
        virtual bool onFocusChanged(bool focus);
        virtual void onBoundChanged(RectF &newBound);
        virtual bool onStateChanged(int newState, int prevState);

        int start_x_, start_y_;

    private:
        int state_;
        int prev_state_;
        bool is_parent_has_focus_;
        RectF bounds_;
    };

}

#endif  // UKIVE_DRAWABLE_DRAWABLE_H_