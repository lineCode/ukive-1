#include "ukive/text/text_action_mode.h"

#include <memory>

#include "utils/weak_bind.hpp"

#include "ukive/menu/menu_impl.h"
#include "ukive/drawable/shape_drawable.h"
#include "ukive/graphics/color.h"
#include "ukive/text/text_action_mode_callback.h"
#include "ukive/animation/view_animator.h"
#include "ukive/window/window.h"
#include "ukive/message/cycler.h"


namespace ukive {

    TextActionMode::TextActionMode(
        Window* window, TextActionModeCallback* callback)
        : is_finished_(true),
          window_(window),
          callback_(callback),
          weak_ref_nest_(this)
    {
        menu_width_ = window->dpToPxX(92);
        menu_item_height_ = window->dpToPxX(36);

        menu_impl_ = new MenuImpl(window);
        menu_impl_->setCallback(this);
        menu_impl_->setMenuItemHeight(menu_item_height_);

        ShapeDrawable* shapeDrawable
            = new ShapeDrawable(ShapeDrawable::ROUND_RECT);
        shapeDrawable->setRadius(2.f);
        shapeDrawable->setSolidEnable(true);
        shapeDrawable->setSolidColor(Color::White);

        inner_window_ = std::make_shared<InnerWindow>(window);
        inner_window_->setElevation(window->dpToPxX(2.f));
        inner_window_->setContentView(menu_impl_);
        inner_window_->setOutsideTouchable(true);
        inner_window_->setBackground(shapeDrawable);
        inner_window_->setWidth(menu_width_);
    }

    TextActionMode::~TextActionMode() {
    }

    void TextActionMode::onCreateMenu(Menu* menu) {
    }

    void TextActionMode::onPrepareMenu(Menu* menu) {
    }

    bool TextActionMode::onMenuItemClicked(Menu* menu, MenuItem* item) {
        return callback_->onActionItemClicked(this, item);
    }

    Menu* TextActionMode::getMenu() {
        return menu_impl_;
    }

    void TextActionMode::invalidateMenu() {
        callback_->onPrepareActionMode(this, menu_impl_);
    }

    void TextActionMode::invalidatePosition() {
        int x = 0, y = 0;
        callback_->onGetContentPosition(&x, &y);

        int window_width = window_->getClientWidth();
        int window_height = window_->getClientHeight();
        if (x + menu_width_ > window_width) {
            x -= menu_width_;
        }

        inner_window_->update(x, y);
    }

    void TextActionMode::show() {
        if (!is_finished_) {
            return;
        }

        is_finished_ = false;

        // 异步打开TextActionMode菜单，以防止在输入事件处理流程中
        // 打开菜单时出现问题。
        window_->getCycler()->post(
            weakref_bind(&TextActionMode::showAsync, weak_ref_nest_.getRef()));
    }

    void TextActionMode::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        menu_impl_->setEnabled(false);
        callback_->onDestroyActionMode(this);
        inner_window_->markDismissing();

        // 异步关闭TextActionMode菜单，以防止在输入事件处理流程中
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


    void TextActionMode::showAsync() {
        int x = 0, y = 0;
        callback_->onGetContentPosition(&x, &y);

        int center_x = 0, center_y = 0;
        int window_width = window_->getClientWidth();
        int window_height = window_->getClientHeight();
        if (x + menu_width_ > window_width) {
            center_x = menu_width_;
            center_y = 0;
            x -= menu_width_;
        }

        inner_window_->dismiss();

        inner_window_->show(x, y);
        inner_window_->getDecorView()->animate()->
            circleReveal(center_x, center_y, 0, 150)->start();
    }

}