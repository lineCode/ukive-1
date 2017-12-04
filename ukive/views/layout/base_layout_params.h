#ifndef UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_PARAMS_H_

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    class BaseLayoutParams : public LayoutParams
    {
    public:
        BaseLayoutParams(int width, int height);
        BaseLayoutParams(const LayoutParams &lp);
        ~BaseLayoutParams();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_BASE_LAYOUT_PARAMS_H_