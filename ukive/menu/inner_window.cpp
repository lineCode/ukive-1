#include "inner_window.h"

#include "ukive/event/input_event.h"
#include "ukive/views/layout/base_layout.h"
#include "ukive/views/layout/base_layout_params.h"
#include "ukive/window/window.h"


namespace ukive {

    InnerWindow::InnerWindow(Window *wnd)
    {
        mParent = wnd;
        mDecorView = nullptr;
        mContentView = nullptr;
        mIsShowing = false;

        mElevation = 0.f;
        mOutsideTouchable = false;
        mDismissByTouchOutside = false;
        mBackgroundDrawable = nullptr;
        mWidth = LayoutParams::FIT_CONTENT;
        mHeight = LayoutParams::FIT_CONTENT;
    }

    InnerWindow::~InnerWindow()
    {
        if (mDecorView && !mIsShowing) {
            delete mDecorView;
        }
    }


    void InnerWindow::createDecorView()
    {
        InnerDecorView *decorView
            = new InnerDecorView(this);
        decorView->addView(
            mContentView, new LayoutParams(mWidth, mHeight));

        decorView->setElevation(mElevation);
        decorView->setBackground(mBackgroundDrawable);
        decorView->setReceiveOutsideInputEvent(!mOutsideTouchable);

        mDecorView = decorView;
    }


    void InnerWindow::setWidth(int width)
    {
        mWidth = width;
    }

    void InnerWindow::setHeight(int height)
    {
        mHeight = height;
    }

    void InnerWindow::setSize(int width, int height)
    {
        mWidth = width;
        mHeight = height;
    }

    void InnerWindow::setElevation(float elevation)
    {
        mElevation = elevation;
    }

    void InnerWindow::setBackground(Drawable *drawable)
    {
        mBackgroundDrawable = drawable;
    }

    void InnerWindow::setOutsideTouchable(bool touchable)
    {
        mOutsideTouchable = touchable;
    }

    void InnerWindow::setDismissByTouchOutside(bool enable)
    {
        mDismissByTouchOutside = enable;
    }

    void InnerWindow::setContentView(View *contentView)
    {
        if (contentView == nullptr)
            throw std::invalid_argument("setContentView: null param");

        mContentView = contentView;
    }


    int InnerWindow::getWidth()
    {
        return mWidth;
    }

    int InnerWindow::getHeight()
    {
        return mHeight;
    }

    float InnerWindow::getElevation()
    {
        return mElevation;
    }

    Drawable *InnerWindow::getBackground()
    {
        return mBackgroundDrawable;
    }

    bool InnerWindow::isOutsideTouchable()
    {
        return mOutsideTouchable;
    }

    bool InnerWindow::isDismissByTouchOutside()
    {
        return mDismissByTouchOutside;
    }

    Window *InnerWindow::getParent()
    {
        return mParent;
    }

    View *InnerWindow::getContentView()
    {
        return mContentView;
    }

    View *InnerWindow::getDecorView()
    {
        return mDecorView;
    }

    bool InnerWindow::isShowing()
    {
        return mIsShowing;
    }


    void InnerWindow::show(int x, int y)
    {
        if (mContentView == nullptr || mIsShowing)
            return;

        if (mDecorView)
            delete mDecorView;

        createDecorView();

        BaseLayoutParams *baselp
            = new BaseLayoutParams(mWidth, mHeight);
        baselp->leftMargin = x;
        baselp->topMargin = y;

        mDecorView->setLayoutParams(baselp);

        mParent->getBaseLayout()->addShade(mDecorView);

        mIsShowing = true;
    }

    void InnerWindow::show(View *anchor, Gravity gravity)
    {
        if (mContentView == nullptr
            || anchor == nullptr || mIsShowing)
            return;

        createDecorView();

        Rect rect = anchor->getBoundInWindow();
    }

    void InnerWindow::update(int x, int y)
    {
        if (mDecorView == nullptr || !mIsShowing)
            return;

        BaseLayoutParams *baselp
            = (BaseLayoutParams*)mDecorView->getLayoutParams();
        baselp->leftMargin = x;
        baselp->topMargin = y;

        mDecorView->setLayoutParams(baselp);
    }

    void InnerWindow::update(View *anchor, Gravity gravity)
    {

    }

    void InnerWindow::dismiss()
    {
        if (mDecorView && mIsShowing)
            mParent->getBaseLayout()->removeShade(mDecorView);
        mIsShowing = false;
    }


    InnerWindow::InnerDecorView::InnerDecorView(InnerWindow *inner)
        :FrameLayout(inner->getParent())
    {
        mInnerWindow = inner;
    }

    InnerWindow::InnerDecorView::InnerDecorView(InnerWindow *inner, int id)
        : FrameLayout(inner->getParent(), id)
    {
        mInnerWindow = inner;
    }

    InnerWindow::InnerDecorView::~InnerDecorView()
    {
    }


    bool InnerWindow::InnerDecorView::onInterceptInputEvent(InputEvent *e)
    {
        return false;
    }

    bool InnerWindow::InnerDecorView::onInputEvent(InputEvent *e)
    {
        if (e->isOutside())
        {
            if (e->getEvent() == InputEvent::EVM_DOWN
                || e->getEvent() == InputEvent::EVM_UP)
            {
                if (mInnerWindow->mDismissByTouchOutside)
                    mInnerWindow->dismiss();
            }
            return true;
        }

        return false;
    }

}