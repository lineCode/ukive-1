#ifndef UKIVE_GRAPHICS_RECT_H_
#define UKIVE_GRAPHICS_RECT_H_


namespace ukive {

    class Point;
    class RectF;

    class Rect {
    public:
        Rect();
        Rect(const Rect& rhs);
        Rect(int x, int y, int width, int height);

        Rect& operator=(const Rect& rhs);
        Rect& operator&(const Rect& rhs);
        Rect& operator|(const Rect& rhs);
        bool operator==(const Rect& rhs) const;
        bool operator!=(const Rect& rhs) const;

        int width() const;
        int height() const;
        bool empty() const;
        bool equal(const Rect& rhs) const;
        bool hit(int x, int y) const;
        bool hit(const Point& p) const;
        bool intersect(const Rect& rect) const;

        void join(const Rect& rhs);
        void same(const Rect& rhs);

        void set(int left, int top, int right, int bottom);

        void insets(const Rect& insets);
        void insets(int left, int top, int right, int bottom);

        void offset(int dx, int dy);

        RectF toRectF();

        int left;
        int top;
        int right;
        int bottom;
    };

    class RectF {
    public:
        RectF();
        RectF(const RectF& rhs);
        RectF(float x, float y, float width, float height);

        RectF& operator=(const RectF& rhs);
        RectF& operator&(const RectF& rhs);
        RectF& operator|(const RectF& rhs);
        bool operator==(const RectF& rhs) const;
        bool operator!=(const RectF& rhs) const;

        float width() const;
        float height() const;
        bool empty() const;
        bool equal(const RectF& rhs) const;
        bool hit(float x, float y) const;
        bool intersect(const RectF& rect) const;

        void join(const RectF& rhs);
        void same(const RectF& rhs);

        void set(float left, float top, float right, float bottom);

        void insets(const RectF& insets);
        void insets(float left, float top, float right, float bottom);

        void offset(float dx, float dy);

        Rect toRect();

        float left;
        float top;
        float right;
        float bottom;
    };

}

#endif  // UKIVE_GRAPHICS_RECT_H_
