#include "rect.h"

#include <algorithm>


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

    Rect& Rect::operator&(const Rect &rhs) {
        same(rhs);
        return *this;
    }

    Rect& Rect::operator|(const Rect &rhs) {
        all(rhs);
        return *this;
    }

    bool Rect::operator==(const Rect &rhs) {
        return equal(rhs);
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

    bool Rect::equal(const Rect &rhs) {
        return (left == rhs.left
            && top == rhs.top
            && right == rhs.right
            && bottom == rhs.bottom);
    }

    bool Rect::intersect(const Rect &rect) {
        return (rect.right > left && rect.left < right)
            && (rect.bottom > top && rect.top < bottom);
    }

    void Rect::all(const Rect &rhs) {
        left = std::min(left, rhs.left);
        top = std::min(top, rhs.top);
        right = std::max(right, rhs.right);
        bottom = std::max(bottom, rhs.bottom);
    }

    void Rect::same(const Rect &rhs) {
        left = std::max(left, rhs.left);
        top = std::max(top, rhs.top);
        right = std::min(right, rhs.right);
        bottom = std::min(bottom, rhs.bottom);

        if (empty()) {
            left = top = right = bottom = 0;
        }
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
        bottom = top + height;
    }

    RectF& RectF::operator=(const RectF &rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
        return *this;
    }

    RectF& RectF::operator&(const RectF &rhs) {
        same(rhs);
        return *this;
    }

    RectF& RectF::operator|(const RectF &rhs) {
        all(rhs);
        return *this;
    }

    bool RectF::operator==(const RectF &rhs) {
        return equal(rhs);
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

    bool RectF::equal(const RectF &rhs) {
        return (left == rhs.left
            && top == rhs.top
            && right == rhs.right
            && bottom == rhs.bottom);
    }

    bool RectF::intersect(const RectF &rect) {
        return (rect.right > left && rect.left < right)
            && (rect.bottom > top && rect.top < bottom);
    }

    void RectF::all(const RectF &rhs) {
        left = std::min(left, rhs.left);
        top = std::min(top, rhs.top);
        right = std::max(right, rhs.right);
        bottom = std::max(bottom, rhs.bottom);
    }

    void RectF::same(const RectF &rhs) {
        left = std::max(left, rhs.left);
        top = std::max(top, rhs.top);
        right = std::min(right, rhs.right);
        bottom = std::min(bottom, rhs.bottom);

        if (empty()) {
            left = top = right = bottom = 0.f;
        }
    }

}