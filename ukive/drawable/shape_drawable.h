#ifndef UKIVE_DRAWABLE_SHAPE_DRAWABLE_H_
#define UKIVE_DRAWABLE_SHAPE_DRAWABLE_H_

#include "ukive/drawable/drawable.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class ShapeDrawable : public Drawable {
    public:
        enum Shape {
            RECT,
            ROUND_RECT,
            OVAL
        };

        explicit ShapeDrawable(Shape shape);
        ~ShapeDrawable();

        void setSize(int width, int height);
        void setRadius(float radius);

        void setSolidEnable(bool enable);
        void setSolidColor(Color color);
        void setStrokeEnable(bool enable);
        void setStrokeWidth(float width);
        void setStrokeColor(Color color);

        void draw(Canvas* canvas) override;

        float getOpacity() const override;
        int getIncWidth() const override;
        int getIncHeight() const override;

    protected:
        bool onStateChanged(int new_state, int prev_state) override;

    private:
        int width_;
        int height_;
        Shape shape_;

        bool has_solid_;
        bool has_stroke_;
        float round_radius_;
        float stroke_width_;
        Color solid_color_;
        Color stroke_color_;
    };

}

#endif  // UKIVE_DRAWABLE_SHAPE_DRAWABLE_H_