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

        void AddToParent(ListAdapter::ViewHolder* holder);
        void AddToParent(ListAdapter::ViewHolder* holder, size_t pos);
        void AddToRecycler(ListAdapter::ViewHolder* holder);
        void RecycleFromParent(View* item_view);
        void RecycleFromParent(size_t start_pos);
        void RecycleFromParent(size_t start_pos, size_t length);
        ListAdapter::ViewHolder* Reuse(int item_id);
        ListAdapter::ViewHolder* Reuse(int item_id, size_t pos);
        ListAdapter::ViewHolder* GetVisible(size_t pos);

        size_t GetVisibleCount();
        size_t GetRecycledCount(int item_id);
        void ClearAll();

    private:
        ViewGroup* parent_;

        std::list<ListAdapter::ViewHolder*> visible_holders_;
        std::map<int, std::vector<ListAdapter::ViewHolder*>> recycled_holders_;
    };

}

#endif  // UKIVE_VIEWS_LIST_VIEW_HOLDER_RECYCLER_H_