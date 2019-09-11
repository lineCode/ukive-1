#ifndef UKIVE_MENU_MENU_IMPL_H_
#define UKIVE_MENU_MENU_IMPL_H_

#include "ukive/menu/menu.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class View;
    class Window;
    class MenuCallback;

    class MenuImpl : public LinearLayout, public Menu, public OnClickListener {
    public:
        explicit MenuImpl(Window* w);

        void setMenuItemHeight(int height);

        void setCallback(MenuCallback* callback) override;
        MenuCallback* getCallback() override;

        MenuItem* addItem(int id, int order, string16 title) override;
        bool removeItem(int id) override;
        bool hasItem(int id) override;
        MenuItem* findItem(int id) override;
        size_t getItemCount() override;

        void onClick(View* v) override;

    private:
        void initMenu();

        int item_height_;
        MenuCallback* callback_;
    };

}

#endif  // UKIVE_MENU_MENU_IMPL_H_