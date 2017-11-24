#ifndef UKIVE_MENU_MENU_CALLBACK_H_
#define UKIVE_MENU_MENU_CALLBACK_H_


namespace ukive {

    class Menu;
    class MenuItem;

    class MenuCallback
    {
    public:
        virtual void onCreateMenu(Menu *menu) = 0;
        virtual void onPrepareMenu(Menu *menu) = 0;
        virtual bool onMenuItemClicked(Menu *menu, MenuItem *item) = 0;
    };

}

#endif  // UKIVE_MENU_MENU_CALLBACK_H_