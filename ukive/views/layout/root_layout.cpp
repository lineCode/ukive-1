#include "root_layout.h"

#include <typeinfo>

#include "ukive/views/layout/frame_layout.h"
#include "ukive/views/layout/root_layout_params.h"
#include "ukive/window/window.h"
#include "ukive/views/debug_view.h"
#include "ukive/views/title_bar/title_bar.h"


namespace {

    const int kTitleBarHeight = 42;

}

namespace ukive {

    RootLayout::RootLayout(Window* w)
        : NonClientLayout(w),
          title_bar_(nullptr),
          debug_view_(nullptr),
          shade_layout_(nullptr),
          content_layout_(nullptr),
          content_view_(nullptr),
          shade_added_(false)
    {
        content_layout_ = new FrameLayout(getWindow());
        addView(content_layout_);

        shade_layout_ = new FrameLayout(getWindow());
    }

    RootLayout::~RootLayout() {
        if (!shade_added_) {
            delete shade_layout_;
        }
    }

    LayoutParams* RootLayout::generateLayoutParams(const LayoutParams& lp) {
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

    void RootLayout::showTitleBar() {
        int title_bar_height = getWindow()->dpToPx(kTitleBarHeight);
        if (!title_bar_) {
            if (content_view_) {
                content_view_->getLayoutParams()->topMargin = title_bar_height;
            }

            title_bar_ = new TitleBar(getWindow());
            title_bar_->setLayoutParams(
                new LayoutParams(LayoutParams::MATCH_PARENT, title_bar_height));
            content_layout_->addView(title_bar_);
        } else {
            if (content_view_) {
                content_view_->getLayoutParams()->topMargin = title_bar_height;
            }
            title_bar_->setVisibility(View::VISIBLE);
        }
    }

    void RootLayout::hideTitleBar() {
        if (title_bar_) {
            if (content_view_) {
                content_view_->getLayoutParams()->topMargin = 0;
            }
            title_bar_->setVisibility(View::VANISHED);
        }
    }

    void RootLayout::removeTitleBar() {
        if (title_bar_) {
            if (content_view_) {
                content_view_->getLayoutParams()->topMargin = 0;
            }
            content_layout_->removeView(title_bar_);
            title_bar_ = nullptr;
        }
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

    void RootLayout::showDebugView() {
        if (!debug_view_) {
            debug_view_ = new DebugView(getWindow());
            debug_view_->setLayoutParams(
                new LayoutParams(
                    LayoutParams::MATCH_PARENT,
                    LayoutParams::MATCH_PARENT));
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
            showDebugView();
        }
    }

    bool RootLayout::isTitleBarShowing() const {
        return title_bar_ && title_bar_->getVisibility() == View::VISIBLE;
    }

    TitleBar* RootLayout::getTitleBar() const {
        return title_bar_;
    }

    DebugView* RootLayout::getDebugView() const {
        return debug_view_;
    }

    void RootLayout::setContent(View* content) {
        if (content == content_view_ || !content) {
            return;
        }

        if (content_view_) {
            content_layout_->removeView(content_view_);
        }

        content_view_ = content;
        auto lp = new LayoutParams(
            LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT);
        if (title_bar_ && title_bar_->getVisibility() != View::VANISHED) {
            lp->topMargin = getWindow()->dpToPx(kTitleBarHeight);
        }

        content_layout_->addView(0, content, lp);
    }


    void RootLayout::requestLayout() {
        NonClientLayout::requestLayout();
        getWindow()->requestLayout();
    }

    View* RootLayout::findViewById(int id) const {
        return content_layout_->findViewById(id);
    }

    HitPoint RootLayout::onNCHitTest(int x, int y) {
        auto hit_point = NonClientLayout::onNCHitTest(x, y);
        if (hit_point == HitPoint::CLIENT &&
            title_bar_ &&
            title_bar_->getVisibility() == View::VISIBLE)
        {
            x -= content_layout_->getLeft();
            y -= content_layout_->getTop();
            auto bounds = title_bar_->getBounds();
            if (bounds.hit(x, y)) {
                hit_point = title_bar_->onNCHitTest(x - bounds.left, y - bounds.top);
            }
        }
        return hit_point;
    }

}