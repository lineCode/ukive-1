#include "menu_item_impl.h"

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/window/window.h"


namespace ukive {

    MenuItemImpl::MenuItemImpl(Window *wnd, int menuId, std::int32_t order)
        :TextView(wnd)
    {
        mOrder = order;
        mMenuId = menuId;
        mIsVisible = true;
        initMenuItem();
    }

    MenuItemImpl::MenuItemImpl(Window *wnd, int id, int menuId, std::int32_t order)
        : TextView(wnd, id)
    {
        mOrder = order;
        mMenuId = menuId;
        mIsVisible = true;
        initMenuItem();
    }

    MenuItemImpl::~MenuItemImpl()
    {
    }


    void MenuItemImpl::initMenuItem()
    {
        setTextSize(13);
        setIsEditable(false);
        setIsSelectable(false);
        setPadding(getWindow()->dpToPx(16), 0, getWindow()->dpToPx(16), 0);
        // TODO: ¶ÀÁ¢ÓÚ Direct2D
        setParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        autoWrap(false);
    }


    void MenuItemImpl::setItemTitle(std::wstring title)
    {
        mTitle = title;
        setText(title);
    }

    void MenuItemImpl::setItemVisible(bool visible)
    {
        if (mIsVisible == visible) return;

        mIsVisible = visible;
        setVisibility(visible ? View::VISIBLE : View::VANISHED);
    }

    void MenuItemImpl::setItemEnabled(bool enable)
    {
        setEnabled(enable);
    }


    int MenuItemImpl::getItemId()
    {
        return mMenuId;
    }

    std::int32_t MenuItemImpl::getItemOrder()
    {
        return mOrder;
    }

    std::wstring MenuItemImpl::getItemTitle()
    {
        return getText();
    }

    bool MenuItemImpl::isItemVisible()
    {
        return mIsVisible;
    }

    bool MenuItemImpl::isItemEnabled()
    {
        return isEnabled();
    }

}