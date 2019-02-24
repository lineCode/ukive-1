#include "spinner_view.h"


namespace ukive {

    SpinnerView::SpinnerView(Window* w)
        : SpinnerView(w, {}) {
    }

    SpinnerView::SpinnerView(Window* w, AttrsRef attrs)
        : TextView(w, attrs) {
    }

    SpinnerView::~SpinnerView() {
    }

}