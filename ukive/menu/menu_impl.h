#ifndef UKIVE_MENU_MENU_IMPL_H_
#define UKIVE_MENU_MENU_IMPL_H_

#include "ukive/menu/menu.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/click_listener.h"


namespace ukive {

    class View;
    class Window;
    class MenuCallback;

    class MenuImpl : public LinearLayout, public Menu, public OnClickListener
    {
    private:
        int mItemHeight;
        MenuCallback *mCallback;

        void initMenu();

    public:
        MenuImpl(Window *wnd);
        MenuImpl(Window *wnd, int id);
        virtual ~MenuImpl();

        void setMenuItemHeight(int height);

        void setCallback(MenuCallback *callback) override;
        MenuCallback *getCallback() override;

        MenuItem *addItem(int id, std::int32_t order, std::wstring title) override;
        bool removeItem(int id) override;
        bool hasItem(int id) override;
        MenuItem *findItem(int id) override;
        std::size_t getItemCount() override;

        void onClick(View *widget) override;
    };

}

#endif  // UKIVE_MENU_MENU_IMPL_H_