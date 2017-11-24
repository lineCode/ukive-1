#ifndef UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_
#define UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_

#include "ukive/animation/animator.h"
#include "ukive/drawable/layer_drawable.h"

#define DOWN_UP_SEC      0.2
#define HOVER_LEAVE_SEC  0.2

#define HOVER_ALPHA  0.07
#define DOWN_ALPHA   0.13


namespace ukive {

    class Window;

    class RippleDrawable
        : public LayerDrawable, public Animator::OnValueChangedListener
    {
    public:
        RippleDrawable(Window *win);
        ~RippleDrawable();

        void setTintColor(D2D1_COLOR_F tint);

        void draw(Canvas *canvas) override;

        float getOpacity() override;

        void onValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard *storyboard,
            IUIAnimationVariable *variable,
            double newValue, double previousValue) override;
        void onIntegerValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard *storyboard,
            IUIAnimationVariable *variable,
            int newValue, int previousValue) override;

    protected:
        bool onStateChanged(int newState, int prevState) override;

    private:
        double mAlpha;
        D2D1_COLOR_F mTintColor;

        Window *owner_win_;

        Animator *mUpAnimator;
        Animator *mDownAnimator;

        Animator *mHoverAnimator;
        Animator *mLeaveAnimator;

        Animator *mRippleAnimator;
    };

}

#endif  // UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_