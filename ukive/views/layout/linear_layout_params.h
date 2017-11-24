#ifndef UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_PARAMS_H_

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    class LinearLayoutParams : public LayoutParams
    {
    public:
        int weight;

    public:
        LinearLayoutParams(int width, int height);
        LinearLayoutParams(LayoutParams *lp);
        virtual ~LinearLayoutParams();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_PARAMS_H_