#ifndef UKIVE_VIEWS_LIST_VIEW_H_
#define UKIVE_VIEWS_LIST_VIEW_H_

#include "ukive/views/layout/view_group.h"


namespace ukive {

    class UListAdapter;


    class UListView : public ViewGroup
    {
    private:
        UListAdapter *mAdapter;

        void initListView();

    public:
        UListView(Window *wnd);
        UListView(Window *wnd, int id);
        virtual ~UListView();

        void setAdapter(UListAdapter *adapter);
    };

}

#endif  // UKIVE_VIEWS_LIST_VIEW_H_