#include "list_view.h"


namespace ukive {

    UListView::UListView(Window *wnd)
        :ViewGroup(wnd)
    {
        initListView();
    }

    UListView::UListView(Window *wnd, int id)
        : ViewGroup(wnd, id)
    {
        initListView();
    }

    UListView::~UListView()
    {
    }


    void UListView::initListView()
    {
        mAdapter = nullptr;
    }


    void UListView::setAdapter(UListAdapter *adapter)
    {
        if (mAdapter != nullptr)
        {
        }

        mAdapter = adapter;
    }

}