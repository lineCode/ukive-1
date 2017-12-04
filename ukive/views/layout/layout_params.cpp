#include "layout_params.h"


namespace ukive {

    LayoutParams::LayoutParams(int width, int height)
    {
        this->width = width;
        this->height = height;
        leftMargin = topMargin = rightMargin = bottomMargin = 0;
    }

    LayoutParams::LayoutParams(const LayoutParams &lp)
    {
        width = lp.width;
        height = lp.height;

        leftMargin = lp.leftMargin;
        rightMargin = lp.rightMargin;
        topMargin = lp.topMargin;
        bottomMargin = lp.bottomMargin;
    }

    LayoutParams::~LayoutParams()
    {
    }

}