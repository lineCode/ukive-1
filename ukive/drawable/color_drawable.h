#ifndef UKIVE_DRAWABLE_COLOR_DRAWABLE_H_
#define UKIVE_DRAWABLE_COLOR_DRAWABLE_H_

#include "ukive/drawable/drawable.h"


namespace ukive {

    class ColorDrawable : public Drawable
    {
    private:
        D2D1_COLOR_F mColor;

    public:
        ColorDrawable(D2D1_COLOR_F color);
        ~ColorDrawable();

        virtual void draw(Canvas *canvas) override;
        virtual float getOpacity() override;

        D2D1_COLOR_F getColor();
    };

}

#endif  // UKIVE_DRAWABLE_COLOR_DRAWABLE_H_