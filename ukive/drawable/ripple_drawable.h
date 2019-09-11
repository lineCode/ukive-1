#ifndef UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_
#define UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_

#include "ukive/animation/animator.h"
#include "ukive/drawable/layer_drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class Window;

    class RippleDrawable :
        public LayerDrawable,
        public AnimationListener
    {
    public:
        RippleDrawable();
        ~RippleDrawable();

        void setTintColor(Color tint);
        void setDrawMaskEnabled(bool enabled);

        void draw(Canvas* canvas) override;

        float getOpacity() override;

        // AnimationListener
        void onAnimationProgress(Animator* animator) override;

    protected:
        void onBoundChanged(const Rect& new_bound) override;
        bool onStateChanged(int new_state, int prev_state) override;

    private:
        double alpha_;
        Color tint_color_;
        bool is_draw_mask_ = true;

        Animator up_animator_;
        Animator down_animator_;
        Animator hover_animator_;
        Animator leave_animator_;
        Animator ripple_animator_;

        std::unique_ptr<Canvas> mask_off_;
        std::unique_ptr<Canvas> content_off_;
    };

}

#endif  // UKIVE_DRAWABLE_RIPPLE_DRAWABLE_H_