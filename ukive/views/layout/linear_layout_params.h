#ifndef UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_PARAMS_H_

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    class LinearLayoutParams : public LayoutParams {
    public:
        LinearLayoutParams(int width, int height);
        explicit LinearLayoutParams(const LayoutParams &lp);
        virtual ~LinearLayoutParams();

        int weight;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LINEAR_LAYOUT_PARAMS_H_