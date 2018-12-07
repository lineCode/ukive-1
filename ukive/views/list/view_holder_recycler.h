#ifndef UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_
#define UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_

#include <list>
#include <map>
#include <vector>

#include "list_adapter.h"


namespace ukive {

    class View;
    class ViewGroup;

    class ViewHolderRecycler {
    public:
        ViewHolderRecycler(ViewGroup* parent);

        void addToParent(ListAdapter::ViewHolder* holder);
        void addToParent(ListAdapter::ViewHolder* holder, int pos);
        void addToRecycler(ListAdapter::ViewHolder* holder);
        void recycleFromParent(View* item_view);
        void recycleFromParent(int start_pos);
        void recycleFromParent(int start_pos, int length);
        ListAdapter::ViewHolder* reuse(int item_id);
        ListAdapter::ViewHolder* reuse(int item_id, int pos);
        ListAdapter::ViewHolder* getVisible(int pos);

        int getVisibleCount();
        int getRecycledCount(int item_id);
        void clearAll();

    private:
        ViewGroup* parent_;

        std::list<ListAdapter::ViewHolder*> visible_holders_;
        std::map<int, std::vector<ListAdapter::ViewHolder*>> recycled_holders_;
    };

}

#endif  // UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_