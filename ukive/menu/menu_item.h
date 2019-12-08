#ifndef UKIVE_MENU_MENU_ITEM_H_
#define UKIVE_MENU_MENU_ITEM_H_

#include "utils/string_utils.h"


namespace ukive {

    class MenuItem {
    public:
        virtual ~MenuItem() = default;

        virtual void setItemTitle(const string16& title) = 0;
        virtual void setItemVisible(bool visible) = 0;
        virtual void setItemEnabled(bool enable) = 0;

        virtual int getItemId() = 0;
        virtual int32_t getItemOrder() = 0;
        virtual string16 getItemTitle() = 0;
        virtual bool isItemVisible() = 0;
        virtual bool isItemEnabled() = 0;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_H_