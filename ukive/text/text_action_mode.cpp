#include "text_action_mode.h"

#include <memory>

#include "ukive/menu/menu_impl.h"
#include "ukive/drawable/shape_drawable.h"
#include "ukive/animation/animator.h"
#include "ukive/utils/executable.h"
#include "ukive/graphics/color.h"
#include "ukive/text/text_action_mode_callback.h"
#include "ukive/animation/view_animator.h"
#include "ukive/window/window.h"
#include "ukive/message/cycler.h"


namespace ukive {

    TextActionMode::TextActionMode(
        Window *window, TextActionModeCallback *callback)
    {
        is_finished_ = true;
        menu_width_ = window->dpToPx(92);
        menu_item_height_ = window->dpToPx(36);

        window_ = window;
        callback_ = callback;

        menu_impl_ = new MenuImpl(window);
        menu_impl_->setCallback(this);
        menu_impl_->setMenuItemHeight(menu_item_height_);

        ShapeDrawable *shapeDrawable
            = new ShapeDrawable(ShapeDrawable::SHAPE_ROUND_RECT);
        shapeDrawable->setRadius(2.f);
        shapeDrawable->setSolidEnable(true);
        shapeDrawable->setSolidColor(Color::White);

        inner_window_ = std::shared_ptr<InnerWindow>(new InnerWindow(window));
        inner_window_->setElevation(window->dpToPx(2.f));
        inner_window_->setContentView(menu_impl_);
        inner_window_->setOutsideTouchable(true);
        inner_window_->setBackground(shapeDrawable);
        inner_window_->setWidth(menu_width_);
    }

    TextActionMode::~TextActionMode()
    {
    }


    void TextActionMode::onCreateMenu(Menu *menu)
    {
    }

    void TextActionMode::onPrepareMenu(Menu *menu)
    {
    }

    bool TextActionMode::onMenuItemClicked(Menu *menu, MenuItem *item)
    {
        return callback_->onActionItemClicked(this, item);
    }


    Menu *TextActionMode::getMenu()
    {
        return menu_impl_;
    }


    void TextActionMode::invalidateMenu()
    {
        callback_->onPrepareActionMode(this, menu_impl_);
    }

    void TextActionMode::invalidatePosition()
    {
        int x, y;
        callback_->onGetContentPosition(&x, &y);

        int windowWidth = window_->getClientWidth();
        int windowHeight = window_->getClientHeight();
        if (x + menu_width_ > windowWidth)
            x -= menu_width_;

        inner_window_->update(x, y);
    }

    void TextActionMode::show() {
        if (!is_finished_) {
            return;
        }

        is_finished_ = false;

        // 异步打开TextActionMode菜单，以防止在输入事件处理流程中
        // 打开菜单时出现问题。
        window_->getCycler()->post(weak_bind(&TextActionMode::showAsync, s_this()));
    }

    void TextActionMode::close() {
        if (is_finished_) {
            return;
        }

        is_finished_ = true;

        menu_impl_->setEnabled(false);
        callback_->onDestroyActionMode(this);

        // 异步关闭TextActionMode菜单，以防止在输入事件处理流程中
        // 关闭菜单时出现问题。
        window_->getCycler()->post(weak_bind(&TextActionMode::closeAsync, s_this()));
    }


    void TextActionMode::showAsync() {
        int x, y;
        callback_->onGetContentPosition(&x, &y);

        int cCenterX = 0, cCenterY = 0;
        int windowWidth = window_->getClientWidth();
        int windowHeight = window_->getClientHeight();
        if (x + menu_width_ > windowWidth)
        {
            cCenterX = menu_width_;
            cCenterY = 0;
            x -= menu_width_;
        }

        inner_window_->show(x, y);

        ViewAnimator::createCirculeReveal(
            inner_window_->getDecorView(), cCenterX, cCenterY, 0, 150)->start();
    }

    void TextActionMode::closeAsync() {
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
        }*animListener = new DismissAnimListener(inner_window_);

        inner_window_->getDecorView()->animate()->
            setDuration(0.1)->alpha(0.f)->setListener(animListener)->start();
    }

}