#include "ukive/animation/interpolator.h"


namespace ukive {

    LinearInterpolator::LinearInterpolator(double final_val)
        : init_val_(0),
          final_val_(final_val) {}

    void LinearInterpolator::setInitVal(double init_val) {
        init_val_ = init_val;
    }

    double LinearInterpolator::interpolate(double progress) {
        if (progress == 1) {
            return final_val_;
        }

        auto distance = final_val_ - init_val_;
        return init_val_ + distance * progress;
    }

}