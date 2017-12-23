#include "ripple_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/bitmap.h"


namespace ukive {

    RippleDrawable::RippleDrawable(Window *win)
        :LayerDrawable(),
        owner_win_(win)
    {
        mAlpha = 0;
        mTintColor = Color(0, 0.f);

        mDownAnimator = new Animator(win->getAnimationManager());
        mUpAnimator = new Animator(win->getAnimationManager());

        mHoverAnimator = new Animator(win->getAnimationManager());
        mLeaveAnimator = new Animator(win->getAnimationManager());

        mRippleAnimator = new Animator(win->getAnimationManager());
    }

    RippleDrawable::~RippleDrawable()
    {
        mRippleAnimator->stop();
        delete mRippleAnimator;

        mHoverAnimator->setOnValueChangedListener(0, nullptr);
        mHoverAnimator->stop();
        delete mHoverAnimator;

        mLeaveAnimator->setOnValueChangedListener(0, nullptr);
        mLeaveAnimator->stop();
        delete mLeaveAnimator;

        mUpAnimator->setOnValueChangedListener(0, nullptr);
        mUpAnimator->stop();
        delete mUpAnimator;

        mDownAnimator->setOnValueChangedListener(0, nullptr);
        mDownAnimator->stop();
        delete mDownAnimator;
    }


    void RippleDrawable::setTintColor(Color tint)
    {
        mTintColor = tint;
    }


    void RippleDrawable::draw(Canvas *canvas)
    {
        auto bound = getBounds();
        Color color(mAlpha, 0.f, 0.f, 0.f);

        //绘制底色、alpha和ripple。
        Canvas offscreen(owner_win_, bound.right - bound.left, bound.bottom - bound.top);
        offscreen.setOpacity(canvas->getOpacity());
        offscreen.fillRect(bound, mTintColor);
        offscreen.fillRect(bound, color);

        if (getState() == STATE_HOVERED
            && getPrevState() == STATE_PRESSED) {

            Color rippleColor = Color(0U,
                (float)mRippleAnimator->getValue(1));

            offscreen.fillCircle(
                start_x_, start_y_,
                mRippleAnimator->getValue(0), rippleColor);
        }
        auto contentBitmap = offscreen.extractBitmap();

        if (mDrawableList.empty()) {
            canvas->drawBitmap(contentBitmap.get());
        }
        else {
            //绘制mask，以该mask确定背景形状以及ripple的扩散边界。
            Canvas offscreen(owner_win_, bound.right - bound.left, bound.bottom - bound.top);
            offscreen.setOpacity(canvas->getOpacity());
            LayerDrawable::draw(&offscreen);
            auto maskBitmap = offscreen.extractBitmap();

            canvas->drawBitmap(maskBitmap.get());
            canvas->fillOpacityMask(
                bound.right - bound.left, bound.bottom - bound.top,
                maskBitmap.get(), contentBitmap.get());
        }
    }

    bool RippleDrawable::onStateChanged(int newState, int prevState)
    {
        switch (newState)
        {
        case STATE_NONE:
        {
            ::OutputDebugStringW(L"STATE_NONE\n");
            if (prevState == STATE_HOVERED)
            {
                mHoverAnimator->stop();
                mAlpha = mHoverAnimator->getValue(0);

                mLeaveAnimator->addVariable(0, mAlpha, 0, 1);
                mLeaveAnimator->setOnValueChangedListener(0, static_cast<OnValueChangedListener*>(this));
                mLeaveAnimator->startTransition(0, Transition::linearTransition(HOVER_LEAVE_SEC, 0));
            }
            else if (prevState == STATE_PRESSED)
            {
                mDownAnimator->stop();
                mAlpha = mDownAnimator->getValue(0);

                mUpAnimator->addVariable(0, mAlpha, 0, 1);
                mUpAnimator->setOnValueChangedListener(0, static_cast<OnValueChangedListener*>(this));
                mUpAnimator->startTransition(0, Transition::linearTransition(DOWN_UP_SEC, 0));
            }
            break;
        }

        case STATE_HOVERED:
            ::OutputDebugStringW(L"STATE_HOVERED\n");
            if (prevState == STATE_NONE)
            {
                mAlpha = 0;
                mHoverAnimator->addVariable(0, 0, 0, 1);
                mHoverAnimator->setOnValueChangedListener(0, static_cast<OnValueChangedListener*>(this));
                mHoverAnimator->startTransition(0, Transition::linearTransition(HOVER_LEAVE_SEC, HOVER_ALPHA));
            }
            else if (prevState == STATE_PRESSED)
            {
                mDownAnimator->stop();
                mAlpha = mDownAnimator->getValue(0);

                mUpAnimator->addVariable(0, mAlpha, 0, 1);
                mUpAnimator->setOnValueChangedListener(0, static_cast<OnValueChangedListener*>(this));
                mUpAnimator->startTransition(0, Transition::linearTransition(DOWN_UP_SEC, HOVER_ALPHA));

                mRippleAnimator->reset();
                mRippleAnimator->addVariable(0, 0, 0, 100);
                mRippleAnimator->addVariable(1, 0.1, 0, 1);
                mRippleAnimator->addTransition(0, Transition::linearTransition(0.5, 100));
                mRippleAnimator->addTransition(1, Transition::linearTransition(0.5, 0));
                mRippleAnimator->start();
            }
            break;

        case STATE_PRESSED:
            ::OutputDebugStringW(L"STATE_PRESSED\n");
            mHoverAnimator->stop();
            mAlpha = mHoverAnimator->getValue(0);

            mDownAnimator->addVariable(0, mAlpha, 0, 1);
            mDownAnimator->setOnValueChangedListener(0, static_cast<OnValueChangedListener*>(this));
            mDownAnimator->startTransition(0, Transition::linearTransition(DOWN_UP_SEC, DOWN_ALPHA));
            break;

        default:
            break;
        }

        return false;
    }

    float RippleDrawable::getOpacity()
    {
        return 1.0f;
    }


    void RippleDrawable::onValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        double newValue, double previousValue)
    {
        mAlpha = newValue;
    }

    void RippleDrawable::onIntegerValueChanged(
        unsigned int varIndex,
        IUIAnimationStoryboard *storyboard,
        IUIAnimationVariable *variable,
        int newValue, int previousValue)
    {

    }

}