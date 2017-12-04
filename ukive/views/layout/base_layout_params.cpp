#include "base_layout_params.h"


namespace ukive {

    BaseLayoutParams::BaseLayoutParams(int width, int height)
        :LayoutParams(width, height)
    {
    }

    BaseLayoutParams::BaseLayoutParams(const LayoutParams &lp)
        : LayoutParams(lp)
    {
    }

    BaseLayoutParams::~BaseLayoutParams()
    {
    }

}