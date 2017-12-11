#include "base_layout.h"

#include <typeinfo>

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
        if (!shade_added_) {
            delete shade_layout_;
        }
    }


    void BaseLayout::initBaseLayout()
    {
        shade_added_ = false;

        content_layout_ = new LinearLayout(getWindow());
        content_layout_->setOrientation(LinearLayout::VERTICAL);
        content_layout_->setLayoutParams(
            new LinearLayoutParams(
                LinearLayoutParams::MATCH_PARENT,
                LinearLayoutParams::MATCH_PARENT));
        addView(content_layout_);

        shade_layout_ = new FrameLayout(getWindow());
        shade_layout_->setCanConsumeMouseEvent(false);
        shade_layout_->setLayoutParams(
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
        shade_layout_->addView(shade);
        if (shade_layout_->getChildCount() == 1) {
            addView(shade_layout_);
            shade_added_ = true;
        }
    }

    void BaseLayout::removeShade(View *shade)
    {
        shade_layout_->removeView(shade);
        if (shade_layout_->getChildCount() == 0) {
            removeView(shade_layout_, false);
            shade_added_ = false;
        }
    }

    void BaseLayout::addContent(View *content)
    {
        content_layout_->addView(content);
    }


    View *BaseLayout::findViewById(int id)
    {
        return content_layout_->findViewById(id);
    }

}