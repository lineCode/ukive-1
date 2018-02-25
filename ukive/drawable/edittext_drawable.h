#ifndef UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_
#define UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_

#include "ukive/drawable/drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class EditTextDrawable : public Drawable
    {
    public:
        EditTextDrawable();
        ~EditTextDrawable();

        bool onFocusChanged(bool focus) override;

        void draw(Canvas *canvas) override;

        float getOpacity() const override;

    private:
        float panel_line_width_;
        Color panel_line_color_;
    };

}

#endif  // UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_