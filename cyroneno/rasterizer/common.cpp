#include "common.h"


namespace cyro {

    int sgn(int val) {
        return (val > 0) - (val < 0);
    }

    int sgn2(int val) {
        return (val >= 0) - (val < 0);
    }

    int sgnd(double val) {
        return (val > 0) - (val < 0);
    }

    int sgnd2(double val) {
        return (val >= 0) - (val < 0);
    }

    int divide(int val) {
        return 2 * val - 1;
    }

}