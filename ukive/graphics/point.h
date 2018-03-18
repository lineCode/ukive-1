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

}

#endif  // UKIVE_GRAPHICS_POINT_H_