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

    class TextBlink : public Executable {
    public:
        TextBlink(View* v);
        ~TextBlink();

        void draw(Canvas* canvas);

        void show();
        void hide();
        void locate(float xCenter, float top, float bottom);

        void setColor(const Color& color);
        void setThickness(float thickness);

        bool isShowing();
        Color getColor();
        float getThickness();

        void run();

    private:
        View* target_view_;
        Cycler* blink_cycler_;

        bool is_located_;
        bool is_cancelled_;
        bool blink_mask_;

        Rect blink_rect_;

        float thickness_;
        Color color_;
    };

}

#endif  // UKIVE_TEXT_TEXT_BLINK_H_