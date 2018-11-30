#include "shape_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/rect.h"


namespace ukive {

    ShapeDrawable::ShapeDrawable(Shape shape)
        :Drawable(),
        width_(-1),
        height_(-1),
        shape_(shape),
        has_solid_(false),
        has_stroke_(false),
        round_radius_(1.f),
        stroke_width_(1.f) {
    }

    ShapeDrawable::~ShapeDrawable() {
    }


    void ShapeDrawable::setSize(int width, int height) {
        width_ = width;
        height_ = height;
    }

    void ShapeDrawable::setRadius(float radius) {
        round_radius_ = radius;
    }

    void ShapeDrawable::setSolidEnable(bool enable) {
        has_solid_ = enable;
    }

    void ShapeDrawable::setSolidColor(Color color) {
        solid_color_ = color;
    }

    void ShapeDrawable::setStrokeEnable(bool enable) {
        has_stroke_ = enable;
    }

    void ShapeDrawable::setStrokeWidth(float width) {
        stroke_width_ = width;
    }

    void ShapeDrawable::setStrokeColor(Color color) {
        stroke_color_ = color;
    }

    void ShapeDrawable::draw(Canvas* canvas) {
        if (!has_solid_ && !has_stroke_) {
            return;
        }

        Rect bound;
        if (width_ == -1 || height_ == -1) {
            bound = getBounds();
        } else {
            bound = Rect(0, 0, width_, height_);
        }

        switch (shape_) {
        case RECT: {
            if (has_solid_) {
                canvas->fillRect(bound.toRectF(), solid_color_);
            }
            if (has_stroke_) {
                canvas->drawRect(bound.toRectF(), stroke_color_);
            }
            break;
        }
        case ROUND_RECT: {
            if (has_solid_) {
                canvas->fillRoundRect(bound.toRectF(), round_radius_, solid_color_);
            }
            if (has_stroke_) {
                canvas->drawRoundRect(bound.toRectF(), stroke_width_, round_radius_, stroke_color_);
            }
            break;
        }
        case OVAL: {
            float cx = (bound.right - bound.left) / 2.f;
            float cy = (bound.bottom - bound.top) / 2.f;

            if (has_solid_) {
                canvas->fillOval(cx, cy, cx, cy, solid_color_);
            }
            if (has_stroke_) {
                canvas->drawOval(cx, cy, cx, cy, stroke_color_);
            }
            break;
        }
        }
    }

    float ShapeDrawable::getOpacity() const {
        if ((has_solid_ && solid_color_.a != 0.f)
            || (has_stroke_ && stroke_color_.a != 0.f && stroke_width_ != 0.f)) {
            return 1.f;
        } else {
            return 0.f;
        }
    }

    int ShapeDrawable::getIncWidth() const {
        return width_;
    }

    int ShapeDrawable::getIncHeight() const {
        return height_;
    }

    bool ShapeDrawable::onStateChanged(int new_state, int prev_state) {
        bool need_redraw = false;
        if (new_state == STATE_DISABLED) {
            if (has_solid_) {
                solid_color_.r *= 0.9f;
                solid_color_.g *= 0.9f;
                solid_color_.b *= 0.9f;
                need_redraw = true;
            }
            if (has_stroke_) {
                stroke_color_.r *= 0.9f;
                stroke_color_.g *= 0.9f;
                stroke_color_.b *= 0.9f;
                need_redraw = true;
            }
        } else if (prev_state == STATE_DISABLED) {
            if (has_solid_) {
                solid_color_.r /= 0.9f;
                solid_color_.g /= 0.9f;
                solid_color_.b /= 0.9f;
                need_redraw = true;
            }
            if (has_stroke_) {
                stroke_color_.r /= 0.9f;
                stroke_color_.g /= 0.9f;
                stroke_color_.b /= 0.9f;
                need_redraw = true;
            }
        }

        return need_redraw;
    }

}