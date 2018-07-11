#include "ukive/views/layout/non_client_layout.h"

#include <algorithm>
#include <Windows.h>

#include "ukive/graphics/canvas.h"
#include "ukive/views/layout/layout_params.h"
#include "ukive/window/window.h"


namespace ukive {

    Color border_color = Color::Blue500;

    NonClientLayout::NonClientLayout(Window* w)
        :ViewGroup(w) {

        if (w->getFrameType() == Window::FRAME_CUSTOM) {
            nc_padding_.set(w->dpToPx(4), w->dpToPx(4), 0, 0);
            sh_padding_.set(w->dpToPx(4), w->dpToPx(4), 0, 0);
        }
    }


    int NonClientLayout::nonClientHitTest(int x, int y) {
        int row = 1, col = 1;

        if (x >= 0 && x < sh_padding_.left) {
            col = 0;
        } else if (x >= getWidth() - sh_padding_.right && x < getWidth()) {
            col = 2;
        }

        if (y >= 0 && y < sh_padding_.top) {
            row = 0;
        } else if (y >= getHeight() - sh_padding_.bottom && y < getHeight()) {
            row = 2;
        }

        LRESULT hitTests[3][3] = {
            { HTTOPLEFT,      HTTOP,      HTTOPRIGHT    },
            { HTLEFT,         HTCLIENT,   HTRIGHT       },
            { HTBOTTOMLEFT,   HTBOTTOM,   HTBOTTOMRIGHT },
        };

        Cursor cursor[3][3] = {
            { Cursor::SIZENWSE,   Cursor::SIZENS,   Cursor::SIZENESW },
            { Cursor::SIZEWE,     Cursor::ARROW,    Cursor::SIZEWE },
            { Cursor::SIZENESW,   Cursor::SIZENS,   Cursor::SIZENWSE },
        };

        setCurrentCursor(cursor[row][col]);

        return hitTests[row][col];
    }

    void NonClientLayout::setNonClientPadding(int left, int top, int right, int bottom) {
        nc_padding_.set(left, top, right - left, bottom - top);
        requestLayout();
    }

    void NonClientLayout::setSizeHandlePadding(int left, int top, int right, int bottom) {
        sh_padding_.set(left, top, right - left, bottom - top);
        requestLayout();
    }

    void NonClientLayout::onMeasure(int width, int height, int widthSpec, int heightSpec) {
        int final_width = 0;
        int final_height = 0;

        int hori_padding = getPaddingLeft() + getPaddingRight() + nc_padding_.left + nc_padding_.right;
        int vert_padding = getPaddingTop() + getPaddingBottom() + nc_padding_.top + nc_padding_.bottom;

        measureChildrenWithMargins(width, height, widthSpec, heightSpec);

        switch (widthSpec) {
        case FIT:
            final_width = getWrappedWidth();
            final_width = std::min(final_width + hori_padding, width);
            final_width = std::max(getMinimumWidth(), final_width);
            break;

        case UNKNOWN:
            final_width = getWrappedWidth();
            final_width = std::max(getMinimumWidth(), final_width);
            break;

        case EXACTLY:
        default:
            final_width = width;
            break;
        }

        switch (heightSpec) {
        case FIT:
            final_height = getWrappedHeight();
            final_height = std::min(final_height + vert_padding, height);
            final_height = std::max(getMinimumHeight(), final_height);
            break;

        case UNKNOWN:
            final_height = getWrappedHeight();
            final_height = std::max(getMinimumHeight(), final_height);
            break;

        case EXACTLY:
        default:
            final_height = height;
            break;
        }

        setMeasuredDimension(final_width, final_height);
    }

    void NonClientLayout::onLayout(
        bool changed, bool sizeChanged,
        int left, int top, int right, int bottom) {
        View* child = nullptr;
        LayoutParams* lp = nullptr;

        for (size_t i = 0; i < getChildCount(); ++i) {
            child = getChildAt(i);
            if (child->getVisibility() != View::VANISHED) {
                lp = child->getLayoutParams();

                int width = child->getMeasuredWidth();
                int height = child->getMeasuredHeight();

                int child_left = getPaddingLeft() + nc_padding_.left + lp->leftMargin;
                int child_top = getPaddingTop() + nc_padding_.top + lp->topMargin;

                child->layout(
                    child_left, child_top,
                    width + child_left,
                    height + child_top);
            }
        }
    }

    void NonClientLayout::onDraw(Canvas* canvas) {
        Rect left_rect = {
            0, 0,
            nc_padding_.left, getHeight() - nc_padding_.bottom };
        canvas->fillRect(left_rect.toRectF(), border_color);

        Rect top_rect = {
            nc_padding_.left, 0,
            getWidth(), nc_padding_.top };
        canvas->fillRect(top_rect.toRectF(), border_color);

        Rect right_rect = {
            getWidth() - nc_padding_.right, nc_padding_.top,
            getWidth(), getHeight() };
        canvas->fillRect(right_rect.toRectF(), border_color);

        Rect bottom_rect = {
            0, getHeight() - nc_padding_.bottom,
            getWidth() - nc_padding_.right, getHeight() };
        canvas->fillRect(bottom_rect.toRectF(), border_color);

        ViewGroup::onDraw(canvas);
    }

}
