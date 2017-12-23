#ifndef UKIVE_VIEWS_SWITCH_VIEW_H_
#define UKIVE_VIEWS_SWITCH_VIEW_H_

#include "ukive/views/view.h"


namespace ukive {

    class SwitchView : public View {
    public:
        SwitchView(Window *wnd);
        SwitchView(Window *wnd, int id);
        ~SwitchView();

    private:
        void initSwitchView();
    };

}

#endif  // UKIVE_VIEWS_SWITCH_VIEW_H_