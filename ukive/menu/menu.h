#ifndef UKIVE_MENU_MENU_H_
#define UKIVE_MENU_MENU_H_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class MenuItem;
    class MenuCallback;

    class Menu
    {
    public:
        virtual void setCallback(MenuCallback *callback) = 0;
        virtual MenuCallback *getCallback() = 0;

        virtual MenuItem *addItem(int id, std::int32_t order, std::wstring title) = 0;
        virtual bool removeItem(int id) = 0;
        virtual bool hasItem(int id) = 0;
        virtual MenuItem *findItem(int id) = 0;
        virtual std::size_t getItemCount() = 0;
    };

}

#endif  // UKIVE_MENU_MENU_H_