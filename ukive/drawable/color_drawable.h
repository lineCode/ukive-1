#ifndef UKIVE_DRAWABLE_COLOR_DRAWABLE_H_
#define UKIVE_DRAWABLE_COLOR_DRAWABLE_H_

#include "ukive/drawable/drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class ColorDrawable : public Drawable
    {
    public:
        ColorDrawable(Color color);
        ~ColorDrawable();

        void draw(Canvas *canvas) override;
        float getOpacity() const override;

        Color getColor() const;

    private:
        Color color_;
    };

}

#endif  // UKIVE_DRAWABLE_COLOR_DRAWABLE_H_