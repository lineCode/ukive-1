#include "overlay_scroll_bar.h"

#include <algorithm>

#include "ukive/graphics/canvas.h"


namespace {
    const int kScrollBarWidth = 8;
    const int kScrollBarMinHeight = 16;
}

namespace ukive {

    OverlayScrollBar::OverlayScrollBar()
        :content_height_(0),
        dragging_deviation_(0.f) {
    }

    OverlayScrollBar::~OverlayScrollBar() {}


    void OverlayScrollBar::Update(int content_height, float percent) {
        content_height_ = content_height;

        if (content_height_ <= 0
            || view_bounds_.empty()
            || view_bounds_.empty()) {
            return;
        }

        int view_height = view_bounds_.height();
        if (view_height >= content_height_) {
            thumb_bounds_.set(0, 0, 0, 0);
            return;
        }

        float scale = view_height / static_cast<float>(content_height_);

        int thumb_width = scrollbar_bounds_.width();
        int thumb_height = std::max(static_cast<int>(view_height * scale), kScrollBarMinHeight);
        int thumb_y = percent * (view_height - thumb_height);

        thumb_bounds_ = Rect(
            view_bounds_.right - thumb_width,
            thumb_y,
            thumb_width, thumb_height);
    }

    void OverlayScrollBar::RegisterScrollHandler(std::function<void(int)> h) {
        scroll_handler_ = h;
    }

    void OverlayScrollBar::SetBounds(int x, int y, int width, int height) {
        view_bounds_.set(x, y, width, height);
        scrollbar_bounds_.set(
            x + width - kScrollBarWidth, y, kScrollBarWidth, height);
    }

    void OverlayScrollBar::SetBounds(const Rect& bounds) {
        view_bounds_ = bounds;
        scrollbar_bounds_.set(
            bounds.right - kScrollBarWidth,
            bounds.top,
            kScrollBarWidth, bounds.height());
    }


    void OverlayScrollBar::OnPaint(Canvas* canvas) {
        if (!thumb_bounds_.empty()) {
            canvas->fillRect(thumb_bounds_.toRectF(), Color::Blue100);
        }
    }

    bool OverlayScrollBar::OnMousePressed(const Point& p) {
        if (scrollbar_bounds_.hit(p)) {
            dragging_start_ = p;
            thumb_bounds_start_ = thumb_bounds_;
            distance_y_prev_ = 0;
            dragging_deviation_ = 0.f;
            return true;
        }
        return false;
    }

    void OverlayScrollBar::OnMouseMoved(const Point& p) {}

    bool OverlayScrollBar::OnMouseDragged(const Point& p) {
        int distance_y = p.y - dragging_start_.y;
        int view_height = view_bounds_.height();

        distance_y = std::max(0 - thumb_bounds_start_.top, distance_y);
        distance_y = std::min(view_height - thumb_bounds_start_.height() - thumb_bounds_start_.top, distance_y);

        thumb_bounds_ = thumb_bounds_start_;
        thumb_bounds_.offset(0, distance_y);

        float scale = static_cast<float>(content_height_ - view_height)
            / (view_height - thumb_bounds_.height());

        float view_dy_float = (distance_y_prev_ - distance_y) * scale;
        int view_dy = view_dy_float;

        dragging_deviation_ += (view_dy_float - view_dy);
        int correction = dragging_deviation_;
        dragging_deviation_ -= correction;

        if ((distance_y < 0 && thumb_bounds_.top == 0)
            || (distance_y > 0 && thumb_bounds_.top == view_height - thumb_bounds_start_.height())) {
            correction += ((distance_y < 0) ? 1 : -1);
        }

        if (scroll_handler_) {
            scroll_handler_(view_dy + correction);
        }

        distance_y_prev_ = distance_y;
        return true;
    }

}