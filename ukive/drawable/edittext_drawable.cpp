#include "edittext_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/color.h"


namespace ukive {

    EditTextDrawable::EditTextDrawable()
        :Drawable()
    {
        mPanelLineWidth = 2;
        mPanelLineColor = Color::Grey400;
    }

    EditTextDrawable::~EditTextDrawable()
    {
    }


    bool EditTextDrawable::onFocusChanged(bool focus)
    {
        if (focus)
        {
            mPanelLineWidth = 2;
            mPanelLineColor = Color::Blue500;
        }
        else
        {
            mPanelLineWidth = 2;
            mPanelLineColor = Color::Grey400;
        }

        return true;
    }


    void EditTextDrawable::draw(Canvas *canvas)
    {
        D2D1_RECT_F bound = getBound();

        canvas->fillRect(
            D2D1::RectF(
                bound.left, bound.bottom - 2,
                bound.right, bound.bottom - (2 - mPanelLineWidth)),
            mPanelLineColor);
    }

    float EditTextDrawable::getOpacity()
    {
        return 1.f;
    }

}