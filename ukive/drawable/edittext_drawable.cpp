#include "edittext_drawable.h"

#include "ukive/graphics/canvas.h"


namespace ukive {

    EditTextDrawable::EditTextDrawable()
        :Drawable()
    {
        panel_line_width_ = 2;
        panel_line_color_ = Color::Grey400;
    }

    EditTextDrawable::~EditTextDrawable()
    {
    }


    bool EditTextDrawable::onFocusChanged(bool focus)
    {
        if (focus)
        {
            panel_line_width_ = 2;
            panel_line_color_ = Color::Blue500;
        }
        else
        {
            panel_line_width_ = 2;
            panel_line_color_ = Color::Grey400;
        }

        return true;
    }


    void EditTextDrawable::draw(Canvas *canvas)
    {
        D2D1_RECT_F bound = getBound();

        canvas->fillRect(
            D2D1::RectF(
                bound.left, bound.bottom - 2,
                bound.right, bound.bottom - (2 - panel_line_width_)),
            panel_line_color_);
    }

    float EditTextDrawable::getOpacity()
    {
        return 1.f;
    }

}