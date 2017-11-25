#ifndef UKIVE_TEXT_TEXT_BLINK_H_
#define UKIVE_TEXT_TEXT_BLINK_H_

#include "ukive/utils/executable.h"
#include "ukive/graphics/rect.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class View;
    class Canvas;
    class Cycler;

    class TextBlink : public Executable
    {
    private:
        View *mTargetView;
        Cycler *mBlinkCycler;

        bool mLocated;
        bool mCancelled;
        bool mBlinkMask;

        Rect mBlinkRect;

        float mThickness;
        Color mColor;

    public:
        TextBlink(View *widget);
        ~TextBlink();

        void draw(Canvas *canvas);

        void show();
        void hide();
        void locate(float xCenter, float top, float bottom);

        void setColor(Color color);
        void setThickness(float thickness);

        bool isShowing();
        Color getColor();
        float getThickness();

        void run();
    };

}

#endif  // UKIVE_TEXT_TEXT_BLINK_H_