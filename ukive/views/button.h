#ifndef UKIVE_VIEWS_BUTTON_H_
#define UKIVE_VIEWS_BUTTON_H_

#include "ukive/views/text_view.h"
#include "ukive/drawable/shape_drawable.h"


namespace ukive {

    class ShapeDrawable;
    class RippleDrawable;

    class Button : public TextView {
    public:
        explicit Button(Window* w);
        Button(Window* w, AttrsRef attrs);

        void setButtonColor(Color color);
        void setButtonShape(ShapeDrawable::Shape shape);

    private:
        void initButton();

        ShapeDrawable* shape_drawable_;
        RippleDrawable* ripple_background_;
    };


    class DropdownButton : public View {
    public:
        explicit DropdownButton(Window* w);

        void onMeasure(int width, int height, int width_mode, int height_mode) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

    private:
        void initButton();

        int tri_length_ = 0;
        int tri_height_ = 0;
        int button_size_ = 0;
        ComPtr<ID2D1PathGeometry> tri_geo_;
    };

}

#endif  // UKIVE_VIEWS_BUTTON_H_