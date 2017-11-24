#ifndef UKIVE_ANIMATION_VIEW_ANIMATOR_H_
#define UKIVE_ANIMATION_VIEW_ANIMATOR_H_

#include "ukive/animation/animator.h"
#include "ukive/animation/transition.h"


namespace ukive {

    class View;

    class ViewAnimator : public Animator::OnValueChangedListener
    {
    public:
        static const int REVEAL_RECT = 0;
        static const int REVEAL_CIRCULE = 1;

    private:
        View *mOwningWidget;
        Animator *mAnimator;

        double mDuration;

        static const int UWIDGET_ANIM_X = 1;
        static const int UWIDGET_ANIM_Y = 2;
        static const int UWIDGET_ANIM_ALPHA = 3;
        static const int UWIDGET_ANIM_SCALE_X = 4;
        static const int UWIDGET_ANIM_SCALE_Y = 5;
        static const int UWIDGET_ANIM_TRANSLATE_X = 6;
        static const int UWIDGET_ANIM_TRANSLATE_Y = 7;
        static const int UWIDGET_ANIM_REVEAL = 8;

    public:
        ViewAnimator(View *widget);
        ~ViewAnimator();

        void start();
        void cancel();
        ViewAnimator *setDuration(double duration);

        ViewAnimator *x(double x);
        ViewAnimator *y(double y);
        ViewAnimator *alpha(double value);
        ViewAnimator *scaleX(double value);
        ViewAnimator *scaleY(double value);
        ViewAnimator *translateX(double value);
        ViewAnimator *translateY(double value);

        ViewAnimator *setListener(Animator::OnAnimatorListener *l);

        static Animator *createRectReveal(
            View *widget, double centerX, double centerY,
            double startWidthRadius, double endWidthRadius,
            double startHeightRadius, double endHeightRadius);
        static Animator *createCirculeReveal(
            View *widget, double centerX, double centerY, double startRadius, double endRadius);

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
    };

}

#endif  // UKIVE_ANIMATION_VIEW_ANIMATOR_H_