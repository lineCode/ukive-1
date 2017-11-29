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
        mIsFinished = true;
        mMenuWidth = window->dpToPx(92);
        mMenuItemHeight = window->dpToPx(36);

        mWindow = window;
        mCallback = callback;

        mMenu = new MenuImpl(window);
        mMenu->setCallback(this);
        mMenu->setMenuItemHeight(mMenuItemHeight);

        ShapeDrawable *shapeDrawable
            = new ShapeDrawable(ShapeDrawable::SHAPE_ROUND_RECT);
        shapeDrawable->setRadius(2.f);
        shapeDrawable->setSolidEnable(true);
        shapeDrawable->setSolidColor(Color::White);

        mInnerWindow = std::shared_ptr<InnerWindow>(new InnerWindow(window));
        mInnerWindow->setElevation(window->dpToPx(2.f));
        mInnerWindow->setContentView(mMenu);
        mInnerWindow->setOutsideTouchable(true);
        mInnerWindow->setBackground(shapeDrawable);
        mInnerWindow->setWidth(mMenuWidth);
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
        return mCallback->onActionItemClicked(this, item);
    }


    Menu *TextActionMode::getMenu()
    {
        return mMenu;
    }


    void TextActionMode::invalidateMenu()
    {
        mCallback->onPrepareActionMode(this, mMenu);
    }

    void TextActionMode::invalidatePosition()
    {
        int x, y;
        mCallback->onGetContentPosition(&x, &y);

        int windowWidth = mWindow->getClientWidth();
        int windowHeight = mWindow->getClientHeight();
        if (x + mMenuWidth > windowWidth)
            x -= mMenuWidth;

        mInnerWindow->update(x, y);
    }

    void TextActionMode::show()
    {
        if (!mIsFinished) return;

        int x, y;
        mCallback->onGetContentPosition(&x, &y);

        int cCenterX = 0, cCenterY = 0;
        int windowWidth = mWindow->getClientWidth();
        int windowHeight = mWindow->getClientHeight();
        if (x + mMenuWidth > windowWidth)
        {
            cCenterX = mMenuWidth;
            cCenterY = 0;
            x -= mMenuWidth;
        }

        mInnerWindow->show(x, y);

        ViewAnimator::createCirculeReveal(
            mInnerWindow->getDecorView(), cCenterX, cCenterY, 0, 150)->start();

        mIsFinished = false;
    }

    void TextActionMode::close()
    {
        if (mIsFinished)
            return;

        mIsFinished = true;

        mMenu->setEnabled(false);

        mCallback->onDestroyActionMode(this);

        //异步关闭TextActionMode菜单，以防止在输入事件处理流程中
        //关闭菜单时出现问题。
        class TextActionModeWorker
            : public Executable
        {
        private:
            std::shared_ptr<InnerWindow> window;
        public:
            TextActionModeWorker(std::shared_ptr<InnerWindow> w)
            {
                window = w;
            }
            void run() override
            {
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
                }*animListener = new DismissAnimListener(window);

                window->getDecorView()->animate()->
                    setDuration(0.1)->alpha(0.f)->setListener(animListener)->start();
            }
        }*worker = new TextActionModeWorker(mInnerWindow);

        mWindow->getCycler()->post(worker);
    }

}