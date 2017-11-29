#include "text_blink.h"

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/message/cycler.h"
#include "ukive/views/view.h"
#include "ukive/window/window.h"


namespace ukive {

    TextBlink::TextBlink(View *widget)
        :mColor(Color::Black)
    {
        mTargetView = widget;
        mBlinkCycler = new Cycler();

        mLocated = false;
        mCancelled = true;
        mBlinkMask = false;

        mThickness = widget->getWindow()->dpToPx(2);
    }

    TextBlink::~TextBlink()
    {
        mCancelled = true;
        mBlinkCycler->removeCallbacks(this);
        delete mBlinkCycler;
    }


    void TextBlink::draw(Canvas *canvas)
    {
        if (mBlinkMask && mLocated)
        {
            canvas->fillRect(D2D1::RectF(
                static_cast<float>(mBlinkRect.left),
                static_cast<float>(mBlinkRect.top),
                static_cast<float>(mBlinkRect.right),
                static_cast<float>(mBlinkRect.bottom)),
                mColor);
        }
    }


    void TextBlink::show()
    {
        if (!mLocated)
            return;

        mCancelled = false;
        mBlinkMask = false;

        mBlinkCycler->removeCallbacks(this);
        mBlinkCycler->post(this);
    }

    void TextBlink::hide()
    {
        mCancelled = true;
        mBlinkCycler->removeCallbacks(this);

        mBlinkMask = false;
        mTargetView->invalidate();
    }

    void TextBlink::locate(float xCenter, float top, float bottom)
    {
        int topInt = std::floor(top);
        int bottomInt = std::ceil(bottom);

        int leftInt = std::round(xCenter - mThickness / 2.f);
        int rightInt = leftInt + mThickness;

        //防止blink被截断。
        if (leftInt < 0)
        {
            rightInt += -leftInt;
            leftInt = 0;
        }

        if (topInt == mBlinkRect.top
            && bottomInt == mBlinkRect.bottom
            && leftInt == mBlinkRect.left
            && rightInt == mBlinkRect.right)
            return;

        bool shouldShow = false;

        if (!mCancelled)
        {
            hide();
            shouldShow = true;
        }

        mBlinkRect.left = leftInt;
        mBlinkRect.top = topInt;
        mBlinkRect.right = rightInt;
        mBlinkRect.bottom = bottomInt;

        mLocated = true;

        if (shouldShow)
            show();
    }


    void TextBlink::setColor(Color color)
    {
        mColor = color;
    }

    void TextBlink::setThickness(float thickness)
    {
        mThickness = thickness;
    }


    bool TextBlink::isShowing()
    {
        return !mCancelled;
    }

    Color TextBlink::getColor()
    {
        return mColor;
    }

    float TextBlink::getThickness()
    {
        return mThickness;
    }


    void TextBlink::run()
    {
        if (!mCancelled)
        {
            mBlinkMask = !mBlinkMask;

            mTargetView->invalidate();

            mBlinkCycler->removeCallbacks(this);
            mBlinkCycler->postDelayed(this, 500);
        }
    }

}