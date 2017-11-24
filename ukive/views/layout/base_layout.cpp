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
    }


    void BaseLayout::initBaseLayout()
    {
        mContentLayout = std::make_shared<LinearLayout>(getWindow());
        mContentLayout->setOrientation(LinearLayout::VERTICAL);
        mContentLayout->setLayoutParams(
            new LinearLayoutParams(
                LinearLayoutParams::MATCH_PARENT,
                LinearLayoutParams::MATCH_PARENT));
        addWidget(mContentLayout);

        mShadeLayout = std::make_shared<FrameLayout>(getWindow());
        mShadeLayout->setCanConsumeMouseEvent(false);
        mShadeLayout->setLayoutParams(
            new LayoutParams(
                LayoutParams::MATCH_PARENT,
                LayoutParams::MATCH_PARENT));
    }


    LayoutParams *BaseLayout::generateLayoutParams(
        LayoutParams *lp)
    {
        return new BaseLayoutParams(lp);
    }

    LayoutParams *BaseLayout::generateDefaultLayoutParams()
    {
        return new BaseLayoutParams(
            BaseLayoutParams::FIT_CONTENT,
            BaseLayoutParams::FIT_CONTENT);
    }

    bool BaseLayout::checkLayoutParams(LayoutParams *lp)
    {
        return typeid(*lp) == typeid(BaseLayoutParams);
    }


    void BaseLayout::addShade(View *shade)
    {
        mShadeLayout->addWidget(shade);
        if (mShadeLayout->getChildCount() == 1)
            addWidget(mShadeLayout);
    }

    void BaseLayout::removeShade(View *shade)
    {
        mShadeLayout->removeWidget(shade);
        if (mShadeLayout->getChildCount() == 0)
            removeWidget(mShadeLayout);
    }

    void BaseLayout::addContent(View *content)
    {
        mContentLayout->addWidget(content);
    }


    View *BaseLayout::findWidgetById(int id)
    {
        return mContentLayout->findWidgetById(id);
    }

}