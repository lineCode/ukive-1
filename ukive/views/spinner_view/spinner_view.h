#ifndef UKIVE_VIEWS_SPINNER_VIEW_SPINNER_VIEW_H_
#define UKIVE_VIEWS_SPINNER_VIEW_SPINNER_VIEW_H_

#include "ukive/menu/inner_window.h"
#include "ukive/views/layout/view_group.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/list/list_view.h"
#include "ukive/utils/weak_ref_nest.h"


namespace ukive {

    class TextView;
    class DropdownButton;
    class SpinnerDropdownView;
    class SpinnerListAdapter;

    class SpinnerView :
        public ViewGroup,
        public OnClickListener,
        public OnInnerWindowEventListener,
        public ListItemSelectedListener {
    public:
        explicit SpinnerView(Window* w);
        SpinnerView(Window* w, AttrsRef attrs);
        ~SpinnerView();

        void addItem(const string16& title);

    protected:
        // ViewGroup
        void onMeasure(int width, int height, int width_mode, int height_mode) override;
        void onLayout(bool changed, bool size_changed, int left, int top, int right, int bottom) override;

        // OnClickListener
        void onClick(View* v) override;

        // OnInnerWindowEventListener
        void onRequestDismissByTouchOutside() override;

        // ListItemSelectedListener
        void onItemSelected(ListAdapter::ViewHolder* holder) override;

    private:
        void initViews();
        void measureViews(int width, int height, int width_mode, int height_mode);

        void show(int x, int y, int width);
        void showAsync(int x, int y, int width);
        void close();

        TextView* text_view_;
        DropdownButton* button_;

        bool is_finished_ = true;
        int min_dropdown_width_ = 0;
        ListView* list_view_ = nullptr;
        SpinnerListAdapter* adapter_ = nullptr;
        std::shared_ptr<InnerWindow> inner_window_;
        WeakRefNest<SpinnerView> weak_ref_nest_;
    };

}

#endif  // UKIVE_VIEWS_SPINNER_VIEW_SPINNER_VIEW_H_