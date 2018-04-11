#include "menu_item_impl.h"

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/window/window.h"


namespace ukive {

    MenuItemImpl::MenuItemImpl(Window* w, int menuId, int32_t order)
        :TextView(w),
        order_(order),
        menu_id_(menuId),
        is_visible_(true) {
        initMenuItem();
    }


    void MenuItemImpl::initMenuItem() {
        setTextSize(13);
        setIsEditable(false);
        setIsSelectable(false);
        setPadding(getWindow()->dpToPx(16), 0, getWindow()->dpToPx(16), 0);
        // TODO: ¶ÀÁ¢ÓÚ Direct2D
        setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        autoWrap(false);
    }


    void MenuItemImpl::setItemTitle(const string16& title) {
        title_ = title;
        setText(title);
    }

    void MenuItemImpl::setItemVisible(bool visible) {
        if (is_visible_ == visible) return;

        is_visible_ = visible;
        setVisibility(visible ? View::VISIBLE : View::VANISHED);
    }

    void MenuItemImpl::setItemEnabled(bool enable) {
        setEnabled(enable);
    }


    int MenuItemImpl::getItemId() {
        return menu_id_;
    }

    int32_t MenuItemImpl::getItemOrder() {
        return order_;
    }

    string16 MenuItemImpl::getItemTitle() {
        return getText();
    }

    bool MenuItemImpl::isItemVisible() {
        return is_visible_;
    }

    bool MenuItemImpl::isItemEnabled() {
        return isEnabled();
    }

}