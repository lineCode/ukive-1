#include "drawable.h"


namespace ukive {

    Drawable::Drawable()
    {
        mStartX = 0.f;
        mStartY = 0.f;
        mParentHasFocus = false;

        mState = STATE_NONE;
        mPrevState = STATE_NONE;
    }


    Drawable::~Drawable()
    {
    }


    void Drawable::setBound(D2D1_RECT_F &rect)
    {
        D2D1_RECT_F oldRect = mRect;
        if (oldRect.left == rect.left
            && oldRect.top == rect.top
            && oldRect.right == rect.right
            && oldRect.bottom == rect.bottom)
            return;

        mRect = rect;
        this->onBoundChanged(mRect);
    }

    void Drawable::setBound(int left, int top, int right, int bottom)
    {
        D2D1_RECT_F oldRect = mRect;
        if (oldRect.left == left
            && oldRect.top == top
            && oldRect.right == right
            && oldRect.bottom == bottom)
            return;

        mRect.left = left;
        mRect.top = top;
        mRect.right = right;
        mRect.bottom = bottom;

        this->onBoundChanged(mRect);
    }

    bool Drawable::setState(int state)
    {
        if (state == mState)
            return false;

        mPrevState = mState;
        mState = state;

        return onStateChanged(state, mPrevState);
    }

    void Drawable::setHotspot(int x, int y)
    {
        mStartX = x;
        mStartY = y;
    }

    bool Drawable::setParentFocus(bool focus)
    {
        if (focus == mParentHasFocus)
            return false;

        mParentHasFocus = focus;
        return onFocusChanged(mParentHasFocus);
    }


    int Drawable::getState()
    {
        return mState;
    }

    int Drawable::getPrevState()
    {
        return mPrevState;
    }

    D2D1_RECT_F &Drawable::getBound()
    {
        return mRect;
    }

    float Drawable::getOpacity()
    {
        return 1.f;
    }


    bool Drawable::onFocusChanged(bool focus)
    {
        return false;
    }

    void Drawable::onBoundChanged(D2D1_RECT_F &newBound)
    {
    }

    bool Drawable::onStateChanged(int newState, int prevState)
    {
        return false;
    }


    int Drawable::getIncWidth()
    {
        return -1;
    }

    int Drawable::getIncHeight()
    {
        return -1;
    }

}