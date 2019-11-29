#include "ukive/views/spinner_view/spinner_list_adapter.h"

#include "utils/stl_utils.h"

#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/text_view.h"
#include "ukive/window/window.h"
#include "ukive/drawable/ripple_drawable.h"


namespace ukive {

    SpinnerListAdapter::SpinnerListAdapter() {}

    ListAdapter::ViewHolder* SpinnerListAdapter::onCreateViewHolder(ViewGroup* parent, int position) {
        auto w = parent->getWindow();

        auto title_tv = new TextView(w);
        title_tv->setPadding(w->dpToPxX(16), w->dpToPxX(8), w->dpToPxX(16), w->dpToPxX(8));
        title_tv->setClickable(true);
        title_tv->setBackground(new RippleDrawable());
        auto title_lp = new LayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::FIT_CONTENT);
        title_tv->setLayoutParams(title_lp);

        return new TextViewHolder(title_tv);
    }

    void SpinnerListAdapter::onBindViewHolder(ViewHolder* holder, int position) {
        auto& data = data_.at(position);
        auto test_holder = reinterpret_cast<TextViewHolder*>(holder);
        test_holder->title_label->setText(data);
    }

    int SpinnerListAdapter::getItemCount() {
        return utl::STLCInt(data_.size());
    }

    void SpinnerListAdapter::addItem(const string16& title) {
        data_.push_back(title);
        notifyDataChanged();
    }

    void SpinnerListAdapter::addItem(int pos, const string16& title) {
        if (pos < 0 || pos >= utl::STLCInt(data_.size())) {
            return;
        }

        data_.insert(data_.begin() + pos, title);
        notifyDataChanged();
    }

    void SpinnerListAdapter::modifyItem(int pos, const string16& title) {
        if (pos < 0 || pos >= utl::STLCInt(data_.size())) {
            return;
        }
        data_[pos] = title;
        notifyDataChanged();
    }

    void SpinnerListAdapter::removeItem(int pos) {
        if (pos < 0 || pos >= utl::STLCInt(data_.size())) {
            return;
        }

        data_.erase(data_.begin() + pos);
        notifyDataChanged();
    }

    string16 SpinnerListAdapter::getItemData(int pos) const {
        return data_[pos];
    }

}