#ifndef UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_
#define UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_

#include "ukive/drawable/drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class Window;

    class EditTextDrawable : public Drawable
    {
    public:
        EditTextDrawable(Window* win);
        ~EditTextDrawable();

        bool onFocusChanged(bool focus) override;

        void draw(Canvas *canvas) override;

        float getOpacity() const override;

    private:
        Window* owner_win_;

        int panel_line_width_;
        Color panel_line_color_;
    };

}

#endif  // UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_