#ifndef CYRONENO_RASTERIZER_COMMON_H_
#define CYRONENO_RASTERIZER_COMMON_H_


namespace cyro {

    /**
     * �� val > 0������ 1��
     * �� val = 0������ 0��
     * �� val < 0������ -1
     */
    int sgn(int val);

    /**
     * �� val >= 0������ 1��
     * �� val < 0������ -1
     */
    int sgn2(int val);

    /**
     * �� val > 0������ 1��
     * �� val = 0������ 0��
     * �� val < 0������ -1
     */
    int sgnd(double val);

    /**
     * �� val >= 0������ 1��
     * �� val < 0������ -1
     */
    int sgnd2(double val);

    /**
     * �� val = 0������ -1��
     * �� val = 1������ 1
     */
    int divide(int val);

}

#endif  // CYRONENO_RASTERIZER_COMMON_H_