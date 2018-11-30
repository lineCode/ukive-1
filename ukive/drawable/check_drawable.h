#ifndef UKIVE_DRAWABLE_CHECK_DRAWABLE_H_
#define UKIVE_DRAWABLE_CHECK_DRAWABLE_H_

#include "ukive/drawable/drawable.h"

#include "ukive/animation/animator.h"


namespace ukive {

    class Window;

    class CheckDrawable : public Drawable {
    public:
        CheckDrawable(Window* w);

        void draw(Canvas* canvas) override;

        void setChecked(bool checked);

        bool onStateChanged(int new_state, int prev_state) override;

    private:
        Window* win_;

        bool checked_;
        std::unique_ptr<Animator> anim_;
    };

}

#endif  // UKIVE_DRAWABLE_CHECK_DRAWABLE_H_