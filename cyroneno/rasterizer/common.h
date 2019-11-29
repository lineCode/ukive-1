#ifndef CYRONENO_RASTERIZER_COMMON_H_
#define CYRONENO_RASTERIZER_COMMON_H_


namespace cyro {

    /**
     * »Ù val > 0£¨∑µªÿ 1£ª
     * »Ù val = 0£¨∑µªÿ 0£ª
     * »Ù val < 0£¨∑µªÿ -1
     */
    int sgn(int val);

    /**
     * »Ù val >= 0£¨∑µªÿ 1£ª
     * »Ù val < 0£¨∑µªÿ -1
     */
    int sgn2(int val);

    /**
     * »Ù val > 0£¨∑µªÿ 1£ª
     * »Ù val = 0£¨∑µªÿ 0£ª
     * »Ù val < 0£¨∑µªÿ -1
     */
    int sgnd(double val);

    /**
     * »Ù val >= 0£¨∑µªÿ 1£ª
     * »Ù val < 0£¨∑µªÿ -1
     */
    int sgnd2(double val);

    /**
     * »Ù val = 0£¨∑µªÿ -1£ª
     * »Ù val = 1£¨∑µªÿ 1
     */
    int divide(int val);

}

#endif  // CYRONENO_RASTERIZER_COMMON_H_