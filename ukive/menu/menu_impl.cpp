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

    MenuImpl::~MenuImpl()
    {
    }


    void MenuImpl::initMenu()
    {
        mCallback = nullptr;
        mItemHeight = LayoutParams::FIT_CONTENT;
    }


    void MenuImpl::setMenuItemHeight(int height)
    {
        mItemHeight = height;
    }


    void MenuImpl::setCallback(MenuCallback *callback)
    {
        mCallback = callback;
    }

    MenuCallback *MenuImpl::getCallback()
    {
        return mCallback;
    }


    MenuItem *MenuImpl::addItem(int id, std::int32_t order, std::wstring title)
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

        this->addView(insertedIndex, widget, new LinearLayoutParams(
            LayoutParams::MATCH_PARENT,
            mItemHeight));
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

    MenuItem *MenuImpl::findItem(int id)
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

    std::size_t MenuImpl::getItemCount()
    {
        return getChildCount();
    }

    void MenuImpl::onClick(View *widget)
    {
        mCallback->onMenuItemClicked(this, dynamic_cast<MenuItem*>(widget));
    }

}