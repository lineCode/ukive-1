#ifndef UKIVE_GRAPHICS_POINT_H_
#define UKIVE_GRAPHICS_POINT_H_


namespace ukive {

    class Point {
    public:
        Point();
        Point(int x, int y);
        Point(const Point& rhs);

        Point& operator=(const Point& rhs);
        bool operator==(const Point& rhs);
        bool operator!=(const Point& rhs);

        void set(int x, int y);

        int x;
        int y;
    };

    class PointF {
    public:
        PointF();
        PointF(float x, float y);
        PointF(const PointF& rhs);

        PointF& operator=(const PointF& rhs);
        bool operator==(const PointF& rhs);
        bool operator!=(const PointF& rhs);

        void set(float x, float y);

        float x;
        float y;
    };

}

#endif  // UKIVE_GRAPHICS_POINT_H_