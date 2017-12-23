#ifndef UKIVE_GRAPHICS_RECT_H_
#define UKIVE_GRAPHICS_RECT_H_


namespace ukive {

    class Rect {
    public:
        Rect();
        Rect(const Rect &rhs);
        Rect(int x, int y, int width, int height);

        Rect& operator=(const Rect &rhs);
        Rect& operator&(const Rect &rhs);
        Rect& operator|(const Rect &rhs);
        bool operator==(const Rect &rhs);
        bool operator!=(const Rect &rhs);

        int width();
        int height();
        bool empty();
        bool equal(const Rect &rhs);
        bool hit(int x, int y);
        bool intersect(const Rect &rect);

        void all(const Rect &rhs);
        void same(const Rect &rhs);

        void insets(const Rect &insets);
        void insets(int left, int top, int right, int bottom);

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
        RectF& operator&(const RectF &rhs);
        RectF& operator|(const RectF &rhs);
        bool operator==(const RectF &rhs);
        bool operator!=(const RectF &rhs);

        float width();
        float height();
        bool empty();
        bool equal(const RectF &rhs);
        bool hit(float x, float y);
        bool intersect(const RectF &rect);

        void all(const RectF &rhs);
        void same(const RectF &rhs);

        void insets(const RectF &insets);
        void insets(float left, float top, float right, float bottom);

        float left;
        float top;
        float right;
        float bottom;
    };

}

#endif  // UKIVE_GRAPHICS_RECT_H_
