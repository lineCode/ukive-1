#include "context_menu.h"

#include "ukive/animation/view_animator.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/menu/context_menu_callback.h"
#include "ukive/menu/menu.h"
#include "ukive/menu/menu_impl.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/executable.h"
#include "ukive/window/window_impl.h"
#include "ukive/window/window.h"


namespace ukive {

    ContextMenu::ContextMenu(
        Window *window, ContextMenuCallback *callback)
    {
        mIsFinished = true;
        mMenuWidth = window->dpToPx(92);
        mMenuItemHeight = window->dpToPx(36);

        mWindow = window;
        mCallback = callback;

        mMenu = new MenuImpl(window);
        mMenu->setCallback(this);
        mMenu->setMenuItemHeight(mMenuItemHeight);

        mInnerWindow = std::make_shared<InnerWindow>(window);
        mInnerWindow->setElevation(1.5f);
        mInnerWindow->setContentView(mMenu);
        mInnerWindow->setOutsideTouchable(true);
        mInnerWindow->setDismissByTouchOutside(true);
        mInnerWindow->setBackground(new ColorDrawable(Color::White));
        mInnerWindow->setWidth(mMenuWidth);
    }

    ContextMenu::~ContextMenu()
    {
    }


    void ContextMenu::onCreateMenu(Menu *menu)
    {
    }

    void ContextMenu::onPrepareMenu(Menu *menu)
    {
    }

    bool ContextMenu::onMenuItemClicked(Menu *menu, MenuItem *item)
    {
        return mCallback->onContextMenuItemClicked(this, item);
    }


    Menu *ContextMenu::getMenu()
    {
        return mMenu;
    }


    void ContextMenu::invalidateMenu()
    {
        if (mIsFinished) return;

        mCallback->onPrepareContextMenu(this, mMenu);
    }

    void ContextMenu::show(int x, int y)
    {
        if (!mIsFinished) return;

        mIsFinished = false;

        //异步打开 ContextMenu，以防止在输入事件处理流程中
        //打开菜单时出现问题。
        mWindow->getCycler()->post(weak_bind(&ContextMenu::showAsync, s_this(), x, y));
    }

    void ContextMenu::close()
    {
        if (mIsFinished)
            return;

        mIsFinished = true;
        mCallback->onDestroyContextMenu(this);

        //异步关闭 ContextMenu，以防止在输入事件处理流程中
        //关闭菜单时出现问题。
        mWindow->getCycler()->post(weak_bind(&ContextMenu::closeAsync, s_this()));
    }

    void ContextMenu::showAsync(int x, int y) {
        mInnerWindow->show(x, y);

        //UWidgetAnimator::createCirculeReveal(
        //mInnerWindow->getDecorView(), cCenterX, cCenterY, 0, 150)->start();
    }

    void ContextMenu::closeAsync() {
        class DismissAnimListener
            : public Animator::OnAnimatorListener
        {
        private:
            std::shared_ptr<InnerWindow> window;
        public:
            DismissAnimListener(std::shared_ptr<InnerWindow> w)
            {
                window = w;
            }
            void onAnimationStart(Animator *animator) {}
            void onAnimationEnd(Animator *animator)
            {
                window->dismiss();
            }
            void onAnimationCancel(Animator *animator)
            {
                window->dismiss();
            }
        }*animListener = new DismissAnimListener(mInnerWindow);

        mInnerWindow->getDecorView()->animate()->
            setDuration(0.1)->alpha(0.f)->setListener(animListener)->start();
    }

}