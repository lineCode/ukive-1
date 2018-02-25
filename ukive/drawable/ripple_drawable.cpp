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


    void RippleDrawable::setTintColor(Color tint) {
        mTintColor = tint;
    }


    void RippleDrawable::draw(Canvas *canvas)
    {
        auto bound = getBounds();
        Color color(mAlpha, 0.f, 0.f, 0.f);

        // 绘制底色、alpha 和 ripple。
        content_off_->beginDraw();
        content_off_->clear();
        content_off_->setOpacity(canvas->getOpacity());
        content_off_->fillRect(bound.toRectF(), mTintColor);
        content_off_->fillRect(bound.toRectF(), color);

        if ((getState() == STATE_HOVERED && getPrevState() == STATE_PRESSED)
            || (getState() == STATE_NONE && getPrevState() == STATE_HOVERED)) {

            Color rippleColor = Color(0U,
                (float)mRippleAnimator->getValue(1));

            content_off_->fillCircle(
                start_x_, start_y_,
                mRippleAnimator->getValue(0), rippleColor);
        }
        content_off_->endDraw();
        auto contentBitmap = content_off_->extractBitmap();

        if (mDrawableList.empty()) {
            canvas->drawBitmap(contentBitmap.get());
        } else {
            // 绘制 mask，以该 mask 确定背景形状以及 ripple 的扩散边界。
            mask_off_->beginDraw();
            mask_off_->clear();
            mask_off_->setOpacity(canvas->getOpacity());
            LayerDrawable::draw(mask_off_.get());
            mask_off_->endDraw();
            auto maskBitmap = mask_off_->extractBitmap();

            canvas->drawBitmap(maskBitmap.get());
            canvas->fillOpacityMask(
                bound.width(), bound.height(),
                maskBitmap.get(), contentBitmap.get());
        }
    }

    void RippleDrawable::onBoundChanged(const Rect& new_bound) {
        LayerDrawable::onBoundChanged(new_bound);

        mask_off_.reset();
        mask_off_ = std::make_unique<Canvas>(
            owner_win_, new_bound.width(), new_bound.height());

        content_off_.reset();
        content_off_ = std::make_unique<Canvas>(
            owner_win_, new_bound.width(), new_bound.height());
    }

    bool RippleDrawable::onStateChanged(int newState, int prevState)
    {
        switch (newState)
        {
        case STATE_NONE:
        {
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