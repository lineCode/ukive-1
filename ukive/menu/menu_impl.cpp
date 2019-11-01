#include "menu_impl.h"

#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/menu/menu_item_impl.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/menu/menu_callback.h"


namespace ukive {

    MenuImpl::MenuImpl(Window* w)
        : LinearLayout(w),
          item_height_(LayoutParams::FIT_CONTENT),
          callback_(nullptr) {
          initMenu();
    }


    void MenuImpl::initMenu() {}

    void MenuImpl::setMenuItemHeight(int height) {
        item_height_ = height;
    }

    void MenuImpl::setCallback(MenuCallback* callback) {
        callback_ = callback;
    }

    MenuCallback* MenuImpl::getCallback() {
        return callback_;
    }

    MenuItem* MenuImpl::addItem(int id, int order, string16 title) {
        auto item = new MenuItemImpl(getWindow(), id, order);
        item->setItemTitle(title);

        View* view = item;
        view->setBackground(new RippleDrawable());
        view->setOnClickListener(this);

        int insertedIndex = getChildCount();
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            MenuItem* childItem = static_cast<MenuItemImpl*>(child);
            if (childItem->getItemOrder() > order) {
                insertedIndex = i;
                break;
            }
        }

        addView(insertedIndex, view, new LinearLayoutParams(
            LayoutParams::MATCH_PARENT,
            item_height_));
        return item;
    }

    bool MenuImpl::removeItem(int id) {
        bool removed = false;

        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            auto item = static_cast<MenuItemImpl*>(child);
            if (item && item->getItemId() == id) {
                removeView(child);

                --i;
                removed = true;
            }
        }

        return removed;
    }

    bool MenuImpl::hasItem(int id) {
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            MenuItem* item = static_cast<MenuItemImpl*>(child);
            if (item && item->getItemId() == id) {
                return true;
            }
        }

        return false;
    }

    MenuItem* MenuImpl::findItem(int id) {
        for (int i = 0; i < getChildCount(); ++i) {
            View* child = getChildAt(i);
            MenuItem* item = static_cast<MenuItemImpl*>(child);
            if (item && item->getItemId() == id) {
                return item;
            }
        }

        return nullptr;
    }

    size_t MenuImpl::getItemCount() {
        return getChildCount();
    }

    void MenuImpl::onClick(View* v) {
        callback_->onMenuItemClicked(this, static_cast<MenuItemImpl*>(v));
    }

}