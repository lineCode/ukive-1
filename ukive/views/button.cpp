﻿#include "button.h"

#include "ukive/drawable/shape_drawable.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    Button::Button(Window *w)
        :TextView(w) {
        initButton();
    }


    void Button::initButton()
    {
        shape_drawable_ = new ShapeDrawable(ShapeDrawable::ROUND_RECT);
        shape_drawable_->setRadius(2.f);
        shape_drawable_->setSolidEnable(true);
        shape_drawable_->setSolidColor(Color::White);

        ripple_background_ = new RippleDrawable(getWindow());
        ripple_background_->addDrawable(shape_drawable_);

        setText(L"Button");
        setTextSize(13);
        setIsEditable(false);
        setIsSelectable(false);

        setPadding(24, 6, 24, 6);
        setBackground(ripple_background_);
        setElevation(2.0f);
    }

    void Button::setButtonColor(Color color)
    {
        shape_drawable_->setSolidColor(color);
    }

}