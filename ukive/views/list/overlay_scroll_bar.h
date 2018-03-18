#ifndef UKIVE_VIEWS_LIST_OVERLAY_SCROLL_BAR_H_
#define UKIVE_VIEWS_LIST_OVERLAY_SCROLL_BAR_H_

#include <functional>

#include "ukive/graphics/point.h"
#include "ukive/graphics/rect.h"


namespace ukive {

    class Canvas;

    class OverlayScrollBar {
    public:
        OverlayScrollBar();
        ~OverlayScrollBar();

        void Update(int content_height, float percent);

        void RegisterScrollHandler(std::function<void(int)> h);

        void SetBounds(int x, int y, int width, int height);
        void SetBounds(const Rect& bounds);

        void OnPaint(Canvas* canvas);
        bool OnMousePressed(const Point& p);
        void OnMouseMoved(const Point& p);
        bool OnMouseDragged(const Point& p);

    private:
        int content_height_;
        int distance_y_prev_;
        float dragging_deviation_;

        Rect view_bounds_;
        Rect scrollbar_bounds_;
        Rect thumb_bounds_;
        Rect thumb_bounds_start_;

        Point dragging_prev_;
        Point dragging_start_;

        std::function<void(int)> scroll_handler_;
    };

}

#endif  // UKIVE_VIEWS_LIST_OVERLAY_SCROLL_BAR_H_