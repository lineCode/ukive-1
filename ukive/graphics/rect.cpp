#include "rect.h"


namespace ukive {

    // Rect
    Rect::Rect()
        :left(0),
        top(0),
        right(0),
        bottom(0) {
    }

    Rect::Rect(const Rect &rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
    }

    Rect::Rect(int x, int y, int width, int height) {
        left = x;
        top = y;
        right = left + width;
        bottom = top + height;
    }

    Rect& Rect::operator=(const Rect &rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
        return *this;
    }

    int Rect::width() {
        return right - left;
    }

    int Rect::height() {
        return bottom - top;
    }

    bool Rect::empty() {
        return (right - left <= 0) || (bottom - top <= 0);
    }


    // RectF
    RectF::RectF()
        :left(0.f),
        top(0.f),
        right(0.f),
        bottom(0.f) {
    }

    RectF::RectF(const RectF &rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
    }

    RectF::RectF(float x, float y, float width, float height) {
        left = x;
        top = y;
        right = left + width;
        height = top + height;
    }

    RectF& RectF::operator=(const RectF &rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
        return *this;
    }

    float RectF::width() {
        return right - left;
    }

    float RectF::height() {
        return bottom - top;
    }

    bool RectF::empty() {
        return (right - left <= 0) || (bottom - top <= 0);
    }

}