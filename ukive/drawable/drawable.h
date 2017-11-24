#ifndef UKIVE_DRAWABLE_DRAWABLE_H_
#define UKIVE_DRAWABLE_DRAWABLE_H_

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class Canvas;

    class Drawable
    {
    private:
        int mState;
        int mPrevState;
        bool mParentHasFocus;
        D2D1_RECT_F mRect;

    protected:
        int mStartX, mStartY;

        virtual bool onFocusChanged(bool focus);
        virtual void onBoundChanged(D2D1_RECT_F &newBound);
        virtual bool onStateChanged(int newState, int prevState);

    public:
        Drawable();
        virtual ~Drawable();

        const static int STATE_NONE = 0;
        const static int STATE_PRESSED = 1;
        const static int STATE_FOCUSED = 2;
        const static int STATE_HOVERED = 3;

        void setBound(D2D1_RECT_F &rect);
        void setBound(int left, int top, int right, int bottom);

        bool setState(int state);
        void setHotspot(int x, int y);
        bool setParentFocus(bool focus);

        int getState();
        int getPrevState();
        D2D1_RECT_F &getBound();

        virtual void draw(Canvas *canvas) = 0;

        virtual float getOpacity();

        virtual int getIncWidth();
        virtual int getIncHeight();
    };

}

#endif  // UKIVE_DRAWABLE_DRAWABLE_H_