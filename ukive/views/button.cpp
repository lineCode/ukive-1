#include "button.h"

#include "ukive/drawable/shape_drawable.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    Button::Button(Window *wnd)
        :TextView(wnd)
    {
        initButton();
    }

    Button::Button(Window *wnd, int id)
        : TextView(wnd, id)
    {
        initButton();
    }


    Button::~Button()
    {
        delete mRippleBackground;
    }


    void Button::initButton()
    {
        mShapeDrawable = new ShapeDrawable(ShapeDrawable::SHAPE_ROUND_RECT);
        mShapeDrawable->setRadius(2.f);
        mShapeDrawable->setSolidEnable(true);
        mShapeDrawable->setSolidColor(Color::White);

        mRippleBackground = new RippleDrawable(mWindow);
        mRippleBackground->addDrawable(mShapeDrawable);

        this->setText(L"UButton");
        this->setTextSize(13);
        this->setIsEditable(false);
        this->setIsSelectable(false);

        this->setPadding(24, 6, 24, 6);
        this->setBackground(mRippleBackground);
        this->setElevation(2.0f);
    }

    void Button::setButtonColor(Color color)
    {
        mShapeDrawable->setSolidColor(color);
    }

}