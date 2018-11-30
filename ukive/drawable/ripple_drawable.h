#ifndef UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_
#define UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_

#include "ukive/animation/animator.h"
#include "ukive/drawable/layer_drawable.h"
#include "ukive/graphics/color.h"

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
        RippleDrawable(Window* win);
        ~RippleDrawable();

        void setTintColor(Color tint);

        void draw(Canvas* canvas) override;

        float getOpacity() override;

        void onValueChanged(
            unsigned int varIndex,
            IUIAnimationStoryboard* storyboard,
            IUIAnimationVariable* variable,
            double newValue, double previousValue) override;

    protected:
        void onBoundChanged(const Rect& new_bound) override;
        bool onStateChanged(int newState, int prevState) override;

    private:
        double alpha_;
        Color tint_color_;

        Window* owner_win_;

        Animator* up_animator_;
        Animator* down_animator_;
        Animator* hover_animator_;
        Animator* leave_animator_;
        Animator* ripple_animator_;

        std::unique_ptr<Canvas> mask_off_;
        std::unique_ptr<Canvas> content_off_;
    };

}

#endif  // UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_