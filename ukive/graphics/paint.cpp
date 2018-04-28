#include "paint.h"


namespace ukive {

    Paint::Paint()
        :color_(Color::Black),
        style_(Style::STROKE),
        bitmap_(nullptr),
        opacity_(1.f),
        stroke_width_(1.f),
        has_stroke_style_(false),
        is_antialiased_(true),
        is_text_antialiased_(true) {}


    void Paint::setStyle(Style s) {
        style_ = s;
    }

    void Paint::setStrokeStyle(const StrokeStyle& s) {
        stroke_style_ = s;
        has_stroke_style_ = true;
    }

    void Paint::setBitmap(Bitmap* b) {
        bitmap_ = b;
    }

    void Paint::setColor(const Color& color) {
        color_ = color;
    }

    void Paint::setOpacity(float opacity) {
        opacity_ = opacity;
    }

    void Paint::setAntialias(bool enabled) {
        is_antialiased_ = enabled;
    }

    void Paint::setTextAntialias(bool enabled) {
        is_text_antialiased_ = enabled;
    }

    void Paint::setStrokeWidth(float width) {
        stroke_width_ = width;
    }

    Paint::Style Paint::getStyle() {
        return style_;
    }

    Paint::StrokeStyle Paint::getStrokeStyle() {
        return stroke_style_;
    }

    Bitmap* Paint::getBitmap() {
        return bitmap_;
    }

    Color Paint::getColor() {
        return color_;
    }

    float Paint::getOpacity() {
        return opacity_;
    }

    bool Paint::hasStrokeStyle() {
        return has_stroke_style_;
    }

    bool Paint::isAntialiased() {
        return is_antialiased_;
    }

    bool Paint::isTextAntialiased() {
        return is_text_antialiased_;
    }

    float Paint::getStrokeWidth() {
        return stroke_width_;
    }

}