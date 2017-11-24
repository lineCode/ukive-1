#include "switch_view.h"


namespace ukive {

    SwitchView::SwitchView(Window *wnd)
        :View(wnd)
    {
        initSwitchView();
    }

    SwitchView::SwitchView(Window *wnd, int id)
        : View(wnd, id)
    {
        initSwitchView();
    }

    SwitchView::~SwitchView()
    {
    }


    void SwitchView::initSwitchView()
    {
    }

}