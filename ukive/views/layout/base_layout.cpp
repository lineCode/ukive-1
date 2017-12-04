#include "base_layout.h"

#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/layout/base_layout_params.h"


namespace ukive {

    BaseLayout::BaseLayout(Window *wnd)
        :FrameLayout(wnd)
    {
        initBaseLayout();
    }

    BaseLayout::BaseLayout(Window *wnd, int id)
        : FrameLayout(wnd, id)
    {
        initBaseLayout();
    }


    BaseLayout::~BaseLayout()
    {
        delete mShadeLayout;
        delete mContentLayout;
    }


    void BaseLayout::initBaseLayout()
    {
        mContentLayout = new LinearLayout(getWindow());
        mContentLayout->setOrientation(LinearLayout::VERTICAL);
        mContentLayout->setLayoutParams(
            new LinearLayoutParams(
                LinearLayoutParams::MATCH_PARENT,
                LinearLayoutParams::MATCH_PARENT));
        addView(mContentLayout);

        mShadeLayout = new FrameLayout(getWindow());
        mShadeLayout->setCanConsumeMouseEvent(false);
        mShadeLayout->setLayoutParams(
            new LayoutParams(
                LayoutParams::MATCH_PARENT,
                LayoutParams::MATCH_PARENT));
    }


    LayoutParams *BaseLayout::generateLayoutParams(const LayoutParams &lp) {
        return new BaseLayoutParams(lp);
    }

    LayoutParams *BaseLayout::generateDefaultLayoutParams()
    {
        return new BaseLayoutParams(
            BaseLayoutParams::MATCH_PARENT,
            BaseLayoutParams::MATCH_PARENT);
    }

    bool BaseLayout::checkLayoutParams(LayoutParams *lp)
    {
        return typeid(*lp) == typeid(BaseLayoutParams);
    }


    void BaseLayout::addShade(View *shade)
    {
        mShadeLayout->addView(shade);
        if (mShadeLayout->getChildCount() == 1)
            addView(mShadeLayout);
    }

    void BaseLayout::removeShade(View *shade)
    {
        mShadeLayout->removeView(shade);
        if (mShadeLayout->getChildCount() == 0)
            removeView(mShadeLayout);
    }

    void BaseLayout::addContent(View *content)
    {
        mContentLayout->addView(content);
    }


    View *BaseLayout::findViewById(int id)
    {
        return mContentLayout->findViewById(id);
    }

}