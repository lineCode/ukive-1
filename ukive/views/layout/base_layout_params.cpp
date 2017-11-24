#include "base_layout_params.h"


namespace ukive {

    BaseLayoutParams::BaseLayoutParams(int width, int height)
        :LayoutParams(width, height)
    {
    }

    BaseLayoutParams::BaseLayoutParams(LayoutParams *lp)
        : LayoutParams(lp)
    {
    }

    BaseLayoutParams::~BaseLayoutParams()
    {
    }

}