#ifndef UKIVE_VIEWS_SWITCH_VIEW_H_
#define UKIVE_VIEWS_SWITCH_VIEW_H_

#include "ukive/views/view.h"


namespace ukive {

    class SwitchView : public View {
    public:
        SwitchView(Window* w);
        ~SwitchView();

        // View
        void onMeasure(int width, int height, int width_mode, int height_mode) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

    private:
    };

}

#endif  // UKIVE_VIEWS_SWITCH_VIEW_H_