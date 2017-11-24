#ifndef UKIVE_MENU_MENU_ITEM_H_
#define UKIVE_MENU_MENU_ITEM_H_

#include "ukive/utils/string_utils.h"


namespace ukive {

    class MenuItem
    {
    public:
        virtual void setItemTitle(std::wstring title) = 0;
        virtual void setItemVisible(bool visible) = 0;
        virtual void setItemEnabled(bool enable) = 0;

        virtual int getItemId() = 0;
        virtual std::int32_t getItemOrder() = 0;
        virtual std::wstring getItemTitle() = 0;
        virtual bool isItemVisible() = 0;
        virtual bool isItemEnabled() = 0;
    };

}

#endif  // UKIVE_MENU_MENU_ITEM_H_