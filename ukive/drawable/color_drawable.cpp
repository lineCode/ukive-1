#include "color_drawable.h"

#include "ukive/graphics/canvas.h"


namespace ukive {

    ColorDrawable::ColorDrawable(Color color)
        :Drawable()
    {
        color_ = color;
    }


    ColorDrawable::~ColorDrawable()
    {
    }


    void ColorDrawable::draw(Canvas *canvas)
    {
        canvas->fillRect(this->getBounds(), color_);
    }


    float ColorDrawable::getOpacity()
    {
        return color_.a;
    }


    Color ColorDrawable::getColor()
    {
        return color_;
    }

}