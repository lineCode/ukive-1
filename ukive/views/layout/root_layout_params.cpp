#include "root_layout_params.h"


namespace ukive {

    RootLayoutParams::RootLayoutParams(int width, int height)
        :LayoutParams(width, height) {}

    RootLayoutParams::RootLayoutParams(const LayoutParams& lp)
        : LayoutParams(lp) {}

    RootLayoutParams::~RootLayoutParams() {}

}