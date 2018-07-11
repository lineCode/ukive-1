#include "root_layout.h"

#include <typeinfo>

#include "ukive/views/layout/frame_layout.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/layout/root_layout_params.h"
#include "ukive/window/window.h"
#include "ukive/views/debug_view.h"


namespace ukive {

    RootLayout::RootLayout(Window* w)
        :NonClientLayout(w),
        debug_view_(nullptr),
        shade_added_(false) {

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

    RootLayout::~RootLayout() {
        if (!shade_added_) {
            delete shade_layout_;
        }
    }


    LayoutParams* RootLayout::generateLayoutParams(const LayoutParams &lp) {
        return new RootLayoutParams(lp);
    }

    LayoutParams* RootLayout::generateDefaultLayoutParams() {
        return new RootLayoutParams(
            RootLayoutParams::MATCH_PARENT,
            RootLayoutParams::MATCH_PARENT);
    }

    bool RootLayout::checkLayoutParams(LayoutParams* lp) {
        return typeid(*lp) == typeid(RootLayoutParams);
    }

    void RootLayout::addShade(View* shade) {
        shade_layout_->addView(shade);
        if (shade_layout_->getChildCount() == 1) {
            if (debug_view_) {
                addView(1, shade_layout_);
            } else {
                addView(shade_layout_);
            }
            shade_added_ = true;
        }
    }

    void RootLayout::removeShade(View* shade) {
        shade_layout_->removeView(shade, false);
        if (shade_layout_->getChildCount() == 0) {
            removeView(shade_layout_, false);
            shade_added_ = false;
        }
    }

    void RootLayout::addDebugView() {
        if (!debug_view_) {
            debug_view_ = new DebugView(getWindow());
            debug_view_->setLayoutParams(
                new LayoutParams(
                    LayoutParams::MATCH_PARENT,
                    LayoutParams::MATCH_PARENT));
            debug_view_->setCanConsumeMouseEvent(false);
            addView(debug_view_);
        }
    }

    void RootLayout::removeDebugView() {
        if (debug_view_) {
            removeView(debug_view_);
            debug_view_ = nullptr;
        }
    }

    void RootLayout::toggleDebugView() {
        if (debug_view_) {
            removeDebugView();
        } else {
            addDebugView();
        }
    }

    DebugView* RootLayout::getDebugView() const {
        return debug_view_;
    }

    void RootLayout::addContent(View* content) {
        content_layout_->addView(content);
    }


    void RootLayout::requestLayout() {
        NonClientLayout::requestLayout();

        getWindow()->requestLayout();
    }

    View* RootLayout::findViewById(int id) {
        return content_layout_->findViewById(id);
    }

}