#ifndef UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_
#define UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_

#include <map>
#include <vector>

#include "list_adapter.h"


namespace ukive {

    class View;
    class ViewGroup;

    class ViewHolderRecycler {
    public:
        explicit ViewHolderRecycler(ViewGroup* parent);

        void addToParent(ListAdapter::ViewHolder* holder);
        void addToParent(ListAdapter::ViewHolder* holder, int pos);
        void addToRecycler(ListAdapter::ViewHolder* holder);
        void recycleFromParent(ListAdapter::ViewHolder* holder);
        ListAdapter::ViewHolder* reuse(int item_id);
        ListAdapter::ViewHolder* reuse(int item_id, int pos);
        int getRecycledCount(int item_id);
        void clear();

    private:
        ViewGroup* parent_;
        std::map<int, std::vector<ListAdapter::ViewHolder*>> recycled_holders_;
    };

}

#endif  // UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_