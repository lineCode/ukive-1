#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_PARAMS_H_

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    class RootLayoutParams : public LayoutParams {
    public:
        RootLayoutParams(int width, int height);
        RootLayoutParams(const LayoutParams& lp);
        ~RootLayoutParams();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_PARAMS_H_