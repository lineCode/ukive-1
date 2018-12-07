#ifndef UKIVE_ANIMATION_VIEW_ANIMATOR_H_
#define UKIVE_ANIMATION_VIEW_ANIMATOR_H_

#include "ukive/animation/animator.h"
#include "ukive/animation/transition.h"


namespace ukive {

    class View;

    class ViewAnimator : public Animator::OnValueChangedListener {
    public:
        ViewAnimator(View* v);
        ~ViewAnimator();

        enum {
            REVEAL_RECT = 0,
            REVEAL_CIRCULE = 1,
        };

        void start();
        void cancel();
        ViewAnimator* setDuration(double duration);

        ViewAnimator* x(double x);
        ViewAnimator* y(double y);
        ViewAnimator* alpha(double value);
        ViewAnimator* scaleX(double value);
        ViewAnimator* scaleY(double value);
        ViewAnimator* translateX(double value);
        ViewAnimator* translateY(double value);

        ViewAnimator* setListener(Animator::OnAnimatorListener* l);

        static Animator* createRectReveal(
            View* v, double centerX, double centerY,
            double startWidthRadius, double endWidthRadius,
            double startHeightRadius, double endHeightRadius);
        static Animator* createCirculeReveal(
            View* v, double centerX, double centerY, double startRadius, double endRadius);

        void onValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            double newValue, double previousValue) override;
        void onIntegerValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            int newValue, int previousValue) override;

    private:
        enum {
            VIEW_ANIM_X = 1,
            VIEW_ANIM_Y,
            VIEW_ANIM_ALPHA,
            VIEW_ANIM_SCALE_X,
            VIEW_ANIM_SCALE_Y,
            VIEW_ANIM_TRANSLATE_X,
            VIEW_ANIM_TRANSLATE_Y,
            VIEW_ANIM_REVEAL,
        };

        double duration_;
        View* owner_view_;
        std::unique_ptr<Animator> animator_;
    };

}

#endif  // UKIVE_ANIMATION_VIEW_ANIMATOR_H_