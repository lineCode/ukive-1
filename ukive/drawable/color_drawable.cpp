#include "color_drawable.h"

#include "ukive/graphics/canvas.h"


namespace ukive {

    ColorDrawable::ColorDrawable(D2D1_COLOR_F color)
        :Drawable()
    {
        mColor = color;
    }


    ColorDrawable::~ColorDrawable()
    {
    }


    void ColorDrawable::draw(Canvas *canvas)
    {
        canvas->fillRect(this->getBound(), mColor);
    }


    float ColorDrawable::getOpacity()
    {
        return mColor.a;
    }


    D2D1_COLOR_F ColorDrawable::getColor()
    {
        return mColor;
    }

}