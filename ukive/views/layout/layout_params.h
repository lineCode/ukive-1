#ifndef UKIVE_VIEWS_LAYOUT_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_LAYOUT_PARAMS_H_


namespace ukive {

    class LayoutParams
    {
    public:
        int width;
        int height;

        int leftMargin;
        int topMargin;
        int rightMargin;
        int bottomMargin;

        const static int FIT_CONTENT = -1;
        const static int MATCH_PARENT = -2;

    public:
        LayoutParams(int width, int height);
        LayoutParams(LayoutParams *lp);
        virtual ~LayoutParams();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_LAYOUT_PARAMS_H_