#include "rect.h"

#include <algorithm>

#include "ukive/graphics/point.h"


namespace ukive {

    ////////////////////////////////////////////////////////////////
    // Rect
    Rect::Rect()
        :left(0),
        top(0),
        right(0),
        bottom(0) {
    }

    Rect::Rect(const Rect& rhs)
        :left(rhs.left),
        top(rhs.top),
        right(rhs.right),
        bottom(rhs.bottom) {}

    Rect::Rect(int x, int y, int width, int height) {
        left = x;
        top = y;
        right = left + width;
        bottom = top + height;
    }

    Rect& Rect::operator=(const Rect& rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
        return *this;
    }

    Rect& Rect::operator&(const Rect& rhs) {
        same(rhs);
        return *this;
    }

    Rect& Rect::operator|(const Rect& rhs) {
        join(rhs);
        return *this;
    }

    bool Rect::operator==(const Rect& rhs) const {
        return equal(rhs);
    }

    bool Rect::operator!=(const Rect& rhs) const {
        return !equal(rhs);
    }

    int Rect::width() const {
        return right - left;
    }

    int Rect::height() const {
        return bottom - top;
    }

    bool Rect::empty() const {
        return (right - left <= 0) || (bottom - top <= 0);
    }

    bool Rect::equal(const Rect& rhs) const {
        return (left == rhs.left
            && top == rhs.top
            && right == rhs.right
            && bottom == rhs.bottom);
    }

    bool Rect::hit(int x, int y) const {
        return (x >= left && x < right
            && y >= top && y < bottom);
    }

    bool Rect::hit(const Point& p) const {
        return (p.x >= left && p.x < right
            && p.y >= top && p.y < bottom);
    }

    bool Rect::intersect(const Rect& rect) const {
        return (rect.right > left && rect.left < right)
            && (rect.bottom > top && rect.top < bottom);
    }

    void Rect::join(const Rect& rhs) {
        left = std::min(left, rhs.left);
        top = std::min(top, rhs.top);
        right = std::max(right, rhs.right);
        bottom = std::max(bottom, rhs.bottom);
    }

    void Rect::same(const Rect& rhs) {
        left = std::max(left, rhs.left);
        top = std::max(top, rhs.top);
        right = std::min(right, rhs.right);
        bottom = std::min(bottom, rhs.bottom);

        if (empty()) {
            left = top = right = bottom = 0;
        }
    }

    void Rect::set(int left, int top, int right, int bottom) {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    void Rect::insets(const Rect& insets) {
        left += insets.left;
        top += insets.top;
        right -= insets.right;
        bottom -= insets.bottom;
    }

    void Rect::insets(int left, int top, int right, int bottom) {
        this->left += left;
        this->top += top;
        this->right -= right;
        this->bottom -= bottom;
    }

    void Rect::offset(int dx, int dy) {
        left += dx;
        right += dx;
        top += dy;
        bottom += dy;
    }

    RectF Rect::toRectF() {
        return RectF(left, top, right - left, bottom - top);
    }


    ////////////////////////////////////////////////////////////////
    // RectF
    RectF::RectF()
        :left(0.f),
        top(0.f),
        right(0.f),
        bottom(0.f) {
    }

    RectF::RectF(const RectF& rhs) {
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

    RectF& RectF::operator=(const RectF& rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
        return *this;
    }

    RectF& RectF::operator&(const RectF& rhs) {
        same(rhs);
        return *this;
    }

    RectF& RectF::operator|(const RectF& rhs) {
        join(rhs);
        return *this;
    }

    bool RectF::operator==(const RectF& rhs) const {
        return equal(rhs);
    }

    bool RectF::operator!=(const RectF& rhs) const {
        return !equal(rhs);
    }

    float RectF::width() const {
        return right - left;
    }

    float RectF::height() const {
        return bottom - top;
    }

    bool RectF::empty() const {
        return (right - left <= 0) || (bottom - top <= 0);
    }

    bool RectF::equal(const RectF& rhs) const {
        return (left == rhs.left
            && top == rhs.top
            && right == rhs.right
            && bottom == rhs.bottom);
    }

    bool RectF::hit(float x, float y) const {
        return (x >= left && x < right
            && y >= top && y < bottom);
    }

    bool RectF::intersect(const RectF& rect) const {
        return (rect.right > left && rect.left < right)
            && (rect.bottom > top && rect.top < bottom);
    }

    void RectF::join(const RectF& rhs) {
        left = std::min(left, rhs.left);
        top = std::min(top, rhs.top);
        right = std::max(right, rhs.right);
        bottom = std::max(bottom, rhs.bottom);
    }

    void RectF::same(const RectF& rhs) {
        left = std::max(left, rhs.left);
        top = std::max(top, rhs.top);
        right = std::min(right, rhs.right);
        bottom = std::min(bottom, rhs.bottom);

        if (empty()) {
            left = top = right = bottom = 0.f;
        }
    }

    void RectF::set(float left, float top, float right, float bottom) {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    void RectF::insets(const RectF& insets) {
        left += insets.left;
        top += insets.top;
        right -= insets.right;
        bottom -= insets.bottom;
    }

    void RectF::insets(float left, float top, float right, float bottom) {
        this->left += left;
        this->top += top;
        this->right -= right;
        this->bottom -= bottom;
    }

    void RectF::offset(float dx, float dy) {
        left += dx;
        right += dx;
        top += dy;
        bottom += dy;
    }

    Rect RectF::toRect() {
        return Rect(left, top, right - left, bottom - top);
    }

}