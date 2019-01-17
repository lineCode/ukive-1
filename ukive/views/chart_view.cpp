#include "ukive/views/chart_view.h"


namespace ukive {

    ChartView::ChartView(Window* w)
        : View(w) {
    }

    void ChartView::onMeasure(int width, int height, int width_mode, int height_mode) {
        int final_width;
        int final_height;

        switch (width_mode) {
        case FIT: final_width = 0; break;
        case UNKNOWN: final_width = 0; break;
        case EXACTLY:
        default: final_width = width; break;
        }

        switch (height_mode) {
        case FIT: final_height = 0; break;
        case UNKNOWN: final_height = 0; break;
        case EXACTLY:
        default: final_height = height; break;
        }

        setMeasuredSize(final_width, final_height);
    }

    void ChartView::onDraw(Canvas* canvas) {
        View::onDraw(canvas);

        auto height = getContentBounds().height();
    }

    void ChartView::addData(float dat) {
        data_.push_back(dat);
    }

    void ChartView::clear() {
        data_.clear();
    }

}