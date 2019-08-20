#ifndef UKIVE_VIEWS_BUTTON_H_
#define UKIVE_VIEWS_BUTTON_H_

#include "ukive/views/text_view.h"
#include "ukive/drawable/shape_drawable.h"


namespace ukive {

    class ShapeDrawable;
    class RippleDrawable;

    class Button : public TextView {
    public:
        explicit Button(Window* w);
        Button(Window* w, AttrsRef attrs);

        void setButtonColor(Color color);
        void setButtonShape(ShapeDrawable::Shape shape);

    private:
        void initButton();

        ShapeDrawable* shape_drawable_;
        RippleDrawable* ripple_background_;
    };

}

#endif  // UKIVE_VIEWS_BUTTON_H_