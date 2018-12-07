#ifndef UKIVE_MENU_MENU_ITEM_IMPL_H_
#define UKIVE_MENU_MENU_ITEM_IMPL_H_

#include "ukive/menu/menu_item.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class Window;

    class MenuItemImpl : public TextView, public MenuItem {
    public:
        MenuItemImpl(Window* w, int menu_id, int32_t order);

        void setItemTitle(const string16& title) override;
        void setItemVisible(bool visible) override;
        void setItemEnabled(bool enable) override;

        int getItemId() override;
        int32_t getItemOrder() override;
        string16 getItemTitle() override;
        bool isItemVisible() override;
        bool isItemEnabled() override;

    private:
        void initMenuItem();

        int menu_id_;
        bool is_visible_;
        string16 title_;
        int32_t order_;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_IMPL_H_