#ifndef UKIVE_MENU_CONTEXT_MENU_H_
#define UKIVE_MENU_CONTEXT_MENU_H_

#include <memory>

#include "ukive/menu/menu_callback.h"
#include "ukive/menu/inner_window.h"


namespace ukive {

    class Window;
    class Menu;
    class MenuImpl;
    class MenuItem;
    class ContextMenuCallback;

    class ContextMenu : public MenuCallback
    {
    private:
        int mMenuWidth;
        int mMenuItemHeight;
        bool mIsFinished;

        Window *mWindow;
        ContextMenuCallback *mCallback;

        MenuImpl *mMenu;
        std::shared_ptr<InnerWindow> mInnerWindow;

    public:
        ContextMenu(Window *window, ContextMenuCallback *callback);
        ~ContextMenu();

        void onCreateMenu(Menu *menu) override;
        void onPrepareMenu(Menu *menu) override;
        bool onMenuItemClicked(Menu *menu, MenuItem *item) override;

        Menu *getMenu();

        void invalidateMenu();

        void show(int x, int y);
        void close();
    };

}

#endif  // UKIVE_MENU_CONTEXT_MENU_H_