﻿#ifndef UKIVE_VIEWS_LAYOUT_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_LAYOUT_PARAMS_H_


namespace ukive {

    class LayoutParams {
    public:
        enum {
            FIT_CONTENT = -1,
            MATCH_PARENT = -2,
        };

        LayoutParams(int width, int height);
        LayoutParams(const LayoutParams& lp) = default;
        virtual ~LayoutParams() = default;

        int width;
        int height;

        int leftMargin;
        int topMargin;
        int rightMargin;
        int bottomMargin;
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LAYOUT_PARAMS_H_