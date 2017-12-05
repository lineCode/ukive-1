#ifndef UKIVE_GRAPHICS_RECT_H_
#define UKIVE_GRAPHICS_RECT_H_


namespace ukive {

    class Rect {
    public:
        Rect();
        Rect(const Rect &rhs);
        Rect(int x, int y, int width, int height);

        Rect& operator=(const Rect &rhs);

        int width();
        int height();
        bool empty();
        bool intersect(const Rect &rect);

        int left;
        int top;
        int right;
        int bottom;
    };

    class RectF {
    public:
        RectF();
        RectF(const RectF &rhs);
        RectF(float x, float y, float width, float height);

        RectF& operator=(const RectF &rhs);

        float width();
        float height();
        bool empty();
        bool intersect(const RectF &rect);

        float left;
        float top;
        float right;
        float bottom;
    };

}

#endif  // UKIVE_GRAPHICS_RECT_H_
