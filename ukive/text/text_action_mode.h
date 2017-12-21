#ifndef UKIVE_TEXT_TEXT_ACTION_MODE_H_
#define UKIVE_TEXT_TEXT_ACTION_MODE_H_

#include <memory>

#include "ukive/menu/menu_callback.h"
#include "ukive/menu/inner_window.h"
#include "ukive/utils/weak_bind.h"


namespace ukive {

    class Window;
    class Menu;
    class MenuImpl;
    class MenuItem;
    class TextActionModeCallback;

    class TextActionMode :
        public MenuCallback,
        public SharedHelper<TextActionMode>
    {
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

    private:
        void showAsync();
        void closeAsync();

        int menu_width_;
        int menu_item_height_;
        bool is_finished_;

        Window *window_;
        TextActionModeCallback *callback_;

        MenuImpl *menu_impl_;
        std::shared_ptr<InnerWindow> inner_window_;
    };

}

#endif  // UKIVE_TEXT_TEXT_ACTION_MODE_H_