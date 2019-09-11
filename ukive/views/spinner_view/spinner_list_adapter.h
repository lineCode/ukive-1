#ifndef UKIVE_VIEWS_SPINNER_VIEW_SPINNER_LIST_ADAPTER_H_
#define UKIVE_VIEWS_SPINNER_VIEW_SPINNER_LIST_ADAPTER_H_

#include <vector>

#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class TextView;

    class SpinnerListAdapter : public ListAdapter {
    public:
        class TextViewHolder : public ViewHolder {
        public:
            explicit TextViewHolder(TextView* v)
                : ViewHolder(reinterpret_cast<View*>(v)),
                  title_label(v) {}

            TextView* title_label;
        };

        SpinnerListAdapter();

        ViewHolder* onCreateViewHolder(ViewGroup* parent, int position) override;
        void onBindViewHolder(ViewHolder* holder, int position) override;
        int getItemCount() override;

        void addItem(const string16& title);
        void addItem(int pos, const string16& title);
        void modifyItem(int pos, const string16& title);
        void removeItem(int pos);
        string16 getItemData(int pos) const;

    private:
        std::vector<string16> data_;
    };

}

#endif  // UKIVE_VIEWS_SPINNER_VIEW_SPINNER_LIST_ADAPTER_H_