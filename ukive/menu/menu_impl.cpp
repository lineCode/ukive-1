#include "menu_impl.h"

#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/menu/menu_item_impl.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/menu/menu_callback.h"


namespace ukive {

    MenuImpl::MenuImpl(Window *wnd)
        :LinearLayout(wnd)
    {
        initMenu();
    }

    MenuImpl::MenuImpl(Window *wnd, int id)
        : LinearLayout(wnd, id)
    {
        initMenu();
    }


    void MenuImpl::initMenu()
    {
        callback_ = nullptr;
        item_height_ = LayoutParams::FIT_CONTENT;
    }


    void MenuImpl::setMenuItemHeight(int height)
    {
        item_height_ = height;
    }


    void MenuImpl::setCallback(MenuCallback *callback)
    {
        callback_ = callback;
    }

    MenuCallback *MenuImpl::getCallback()
    {
        return callback_;
    }


    MenuItem* MenuImpl::addItem(int id, int order, string16 title)
    {
        MenuItem *item = new MenuItemImpl(getWindow(), id, order);
        item->setItemTitle(title);

        View *widget = dynamic_cast<View*>(item);
        widget->setBackground(new RippleDrawable(getWindow()));
        widget->setOnClickListener(this);

        int insertedIndex = getChildCount();
        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            MenuItem *childItem = dynamic_cast<MenuItem*>(child);
            if (childItem->getItemOrder() > order)
            {
                insertedIndex = i;
                break;
            }
        }

        addView(insertedIndex, widget, new LinearLayoutParams(
            LayoutParams::MATCH_PARENT,
            item_height_));
        return item;
    }

    bool MenuImpl::removeItem(int id)
    {
        bool removed = false;

        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            MenuItem *item = dynamic_cast<MenuItem*>(child);
            if (item && item->getItemId() == id)
            {
                removeView(child);

                --i;
                removed = true;
            }
        }

        return removed;
    }

    bool MenuImpl::hasItem(int id)
    {
        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            MenuItem *item = dynamic_cast<MenuItem*>(child);
            if (item && item->getItemId() == id)
                return true;
        }

        return false;
    }

    MenuItem* MenuImpl::findItem(int id)
    {
        for (std::size_t i = 0; i < getChildCount(); ++i)
        {
            View *child = getChildAt(i);
            MenuItem *item = dynamic_cast<MenuItem*>(child);
            if (item && item->getItemId() == id)
                return item;
        }

        return nullptr;
    }

    size_t MenuImpl::getItemCount()
    {
        return getChildCount();
    }

    void MenuImpl::onClick(View *v)
    {
        callback_->onMenuItemClicked(this, dynamic_cast<MenuItem*>(v));
    }

}