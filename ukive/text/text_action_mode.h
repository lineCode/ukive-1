#ifndef UKIVE_TEXT_TEXT_ACTION_MODE_H_
#define UKIVE_TEXT_TEXT_ACTION_MODE_H_

#include <memory>

#include "ukive/menu/menu_callback.h"
#include "ukive/menu/inner_window.h"


namespace ukive {

    class Window;
    class Menu;
    class MenuImpl;
    class MenuItem;
    class TextActionModeCallback;

    class TextActionMode : public MenuCallback
    {
    private:
        int mMenuWidth;
        int mMenuItemHeight;
        bool mIsFinished;

        Window *mWindow;
        TextActionModeCallback *mCallback;

        MenuImpl *mMenu;
        std::shared_ptr<InnerWindow> mInnerWindow;

    public:
        TextActionMode(Window *window, TextActionModeCallback *callback);
        ~TextActionMode();

        void onCreateMenu(Menu *menu) override;
        void onPrepareMenu(Menu *menu) override;
        bool onMenuItemClicked(Menu *menu, MenuItem *item) override;

        Menu *getMenu();

        void invalidateMenu();
        void invalidatePosition();

        void show();
        void close();
    };

}

#endif  // UKIVE_TEXT_TEXT_ACTION_MODE_H_