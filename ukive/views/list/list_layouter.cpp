#include "ukive/views/list/list_layouter.h"

namespace ukive {

    ListLayouter::ListLayouter() {}

    void ListLayouter::bind(ListView* parent, ListAdapter* adapter) {
        parent_ = parent;
        adapter_ = adapter;
    }

    bool ListLayouter::isAvailable() const {
        return parent_ && adapter_;
    }

}