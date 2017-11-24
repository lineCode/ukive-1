#ifndef UKIVE_MENU_MENU_ITEM_IMPL_H_
#define UKIVE_MENU_MENU_ITEM_IMPL_H_

#include "ukive/menu/menu_item.h"
#include "ukive/views/text_view.h"


namespace ukive {

    class Window;

    class MenuItemImpl : public TextView, public MenuItem
    {
    private:
        int mMenuId;
        bool mIsVisible;
        std::wstring mTitle;
        std::int32_t mOrder;

        void initMenuItem();

    public:
        MenuItemImpl(Window *wnd, int menuId, std::int32_t order);
        MenuItemImpl(Window *wnd, int id, int menuId, std::int32_t order);
        virtual ~MenuItemImpl();

        void setItemTitle(std::wstring title) override;
        void setItemVisible(bool visible) override;
        void setItemEnabled(bool enable) override;

        int getItemId() override;
        std::int32_t getItemOrder() override;
        std::wstring getItemTitle() override;
        bool isItemVisible() override;
        bool isItemEnabled() override;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_IMPL_H_