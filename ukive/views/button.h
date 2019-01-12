#ifndef UKIVE_VIEWS_BUTTON_H_
#define UKIVE_VIEWS_BUTTON_H_

#include "ukive/views/text_view.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class ShapeDrawable;
    class RippleDrawable;

    class Button : public TextView {
    public:
        explicit Button(Window* w);

        void setButtonColor(Color color);

    private:
        void initButton();

        ShapeDrawable* shape_drawable_;
        RippleDrawable* ripple_background_;
    };

}

#endif  // UKIVE_VIEWS_BUTTON_H_