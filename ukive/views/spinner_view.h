#ifndef UKIVE_VIEWS_SPINNER_VIEW_H_
#define UKIVE_VIEWS_SPINNER_VIEW_H_

#include "ukive/views/text_view.h"


namespace ukive {

    class SpinnerView : public TextView {
    public:
        explicit SpinnerView(Window* w);
        SpinnerView(Window* w, AttrsRef attrs);
        ~SpinnerView();
    };

}

#endif  // UKIVE_VIEWS_SPINNER_VIEW_H_