#ifndef UKIVE_VIEWS_BUTTON_H_
#define UKIVE_VIEWS_BUTTON_H_

#include "ukive/views/text_view.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class ShapeDrawable;
    class RippleDrawable;

    class Button : public TextView
    {
    private:
        ShapeDrawable *mShapeDrawable;
        RippleDrawable *mRippleBackground;

        void initButton();

    public:
        Button(Window *wnd);
        Button(Window *wnd, int id);
        virtual ~Button();

        void setButtonColor(Color color);
    };

}

#endif  // UKIVE_VIEWS_BUTTON_H_