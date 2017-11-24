#ifndef UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_
#define UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_

#include "ukive/drawable/drawable.h"


namespace ukive {

    class EditTextDrawable : public Drawable
    {
    private:
        float mPanelLineWidth;
        D2D1_COLOR_F mPanelLineColor;

    public:
        EditTextDrawable();
        ~EditTextDrawable();

        bool onFocusChanged(bool focus) override;

        void draw(Canvas *canvas) override;

        float getOpacity() override;
    };

}

#endif  // UKIVE_DRAWABLE_EDITTEXT_DRAWABLE_H_