#include "spinner_view.h"


namespace ukive {

    SpinnerView::SpinnerView(Window *wnd)
        :TextView(wnd)
    {
    }

    SpinnerView::SpinnerView(Window *wnd, int id)
        : TextView(wnd, id)
    {

    }

    SpinnerView::~SpinnerView()
    {
    }

}