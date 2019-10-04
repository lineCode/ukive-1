#include "context_menu.h"

#include "ukive/animation/view_animator.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/menu/context_menu_callback.h"
#include "ukive/menu/menu.h"
#include "ukive/menu/menu_impl.h"
#include "ukive/graphics/color.h"
#include "ukive/window/window_impl.h"
#include "ukive/window/window.h"
#include "ukive/utils/weak_bind.h"


namespace ukive {

    ContextMenu::ContextMenu(
        Window* window, ContextMenuCallback* callback)
        : is_finished_(true),
          window_(window),
          callback_(callback),
          weak_ref_nest_(this)
    {
        menu_width_ = int(window->dpToPxX(92));
        menu_item_height_ = int(window->dpToPxX(36));

        menu_ = new MenuImpl(window);
        menu_->setCallback(this);
        menu_->setMenuItemHeight(menu_item_height_);

        inner_window_ = std::make_shared<InnerWindow>(window);
        inner_window_->setElevation(1.5f);
        inner_window_->setContentView(menu_);
        inner_window_->setOutsideTouchable(true);
        inner_window_->setDismissByTouchOutside(true);
        inner_window_->setBackground(new ColorDrawable(Color::White));
        inner_window_->setWidth(menu_width_);
    }

    ContextMenu::~ContextMenu() {}

    void ContextMenu::onCreateMenu(Menu* menu) {}

    void ContextMenu::onPrepareMenu(Menu* menu) {}

    bool ContextMenu::onMenuItemClicked(Menu* menu, MenuItem* item) {
        return callback_->onContextMenuItemClicked(this, item);
    }

    Menu* ContextMenu::getMenu() {
        return menu_;
    }

    void ContextMenu::invalidateMenu() {
        if (is_finished_) return;

        callback_->onPrepareContextMenu(this, menu_);
    }

    void ContextMenu::show(int x, int y) {
        if (!is_finished_) return;

        is_finished_ = false;

        // 异步打开 ContextMenu，以防止在输入事件处理流程中
        // 打开菜单时出现问题。
        window_->getCycler()->post(
            weakref_bind(&ContextMenu::showAsync, weak_ref_nest_.getRef(), x, y));
    }

    void ContextMenu::close() {
        if (is_finished_)
            return;

        is_finished_ = true;
        callback_->onDestroyContextMenu(this);

        // 异步关闭 ContextMenu，以防止在输入事件处理流程中
        // 关闭菜单时出现问题。
        std::weak_ptr<InnerWindow> ptr = inner_window_;
        inner_window_->getDecorView()->animate()->
            setDuration(100)->alpha(0.f)->setFinishedHandler(
                [ptr](AnimationDirector* director)
        {
            auto window = ptr.lock();
            if (window) {
                window->dismiss();
            }
        })->start();
    }

    void ContextMenu::showAsync(int x, int y) {
        inner_window_->show(x, y);

        //ViewAnimator::createCirculeReveal(
        //inner_window_->getDecorView(), cCenterX, cCenterY, 0, 150)->start();
    }

}