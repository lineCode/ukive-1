﻿#include "button.h"

#include "ukive/window/window.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/graphics/color.h"

#include "oigka/layout_constants.h"


namespace ukive {

    Button::Button(Window* w)
        : Button(w, {})
    {}

    Button::Button(Window *w, AttrsRef attrs)
        : TextView(w, attrs)
    {
        if (attrs.find(oigka::kAttrTextViewText) == attrs.end()) {
            setText(L"Button");
        }
        setTextSize(13);
        setIsEditable(false);
        setIsSelectable(false);

        initButton();
    }

    void Button::initButton() {
        shape_drawable_ = new ShapeDrawable(ShapeDrawable::ROUND_RECT);
        shape_drawable_->setRadius(getWindow()->dpToPx(2.f));
        shape_drawable_->setSolidEnable(true);
        shape_drawable_->setSolidColor(Color::White);

        ripple_background_ = new RippleDrawable();
        ripple_background_->addDrawable(shape_drawable_);

        setPadding(
            getWindow()->dpToPx(24),
            getWindow()->dpToPx(6),
            getWindow()->dpToPx(24),
            getWindow()->dpToPx(6));
        setBackground(ripple_background_);
        setElevation(getWindow()->dpToPx(2.0f));
    }

    void Button::setButtonColor(Color color) {
        shape_drawable_->setSolidColor(color);
        invalidate();
    }

    void Button::setButtonShape(ShapeDrawable::Shape shape) {
        switch (shape) {
        case ShapeDrawable::OVAL:
            setOutline(OUTLINE_OVAL);
            break;

        case ShapeDrawable::RECT:
        case ShapeDrawable::ROUND_RECT:
        default:
            setOutline(OUTLINE_RECT);
            break;
        }
        shape_drawable_->setShape(shape);
        invalidate();
    }

}