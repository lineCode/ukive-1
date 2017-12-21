#ifndef UKIVE_MENU_CONTEXT_MENU_H_
#define UKIVE_MENU_CONTEXT_MENU_H_

#include <memory>

#include "ukive/menu/menu_callback.h"
#include "ukive/menu/inner_window.h"
#include "ukive/utils/weak_bind.h"


namespace ukive {

    class Window;
    class Menu;
    class MenuImpl;
    class MenuItem;
    class ContextMenuCallback;

    class ContextMenu :
        public MenuCallback,
        public SharedHelper<ContextMenu>
    {
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

    private:
        void showAsync(int x, int y);
        void closeAsync();

        int mMenuWidth;
        int mMenuItemHeight;
        bool mIsFinished;

        Window *mWindow;
        ContextMenuCallback *mCallback;

        MenuImpl *mMenu;
        std::shared_ptr<InnerWindow> mInnerWindow;
    };

}

#endif  // UKIVE_MENU_CONTEXT_MENU_H_