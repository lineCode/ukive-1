#ifndef UKIVE_VIEWS_CHART_VIEW_H_
#define UKIVE_VIEWS_CHART_VIEW_H_

#include <vector>

#include "ukive/views/view.h"


namespace ukive {

    class ChartView : public View {
    public:
        explicit ChartView(Window* w);
        ChartView(Window* w, AttrsRef attrs);

        void addData(float dat);
        void clear();

    protected:
        void onMeasure(int width, int height, int width_mode, int height_mode) override;
        void onDraw(Canvas* canvas) override;

    private:
        std::vector<float> data_;
    };

}

#endif  // UKIVE_VIEWS_CHART_VIEW_H_