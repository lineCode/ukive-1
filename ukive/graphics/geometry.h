#ifndef UKIVE_GRAPHICS_GEOMETRY_H_
#define UKIVE_GRAPHICS_GEOMETRY_H_

#include "ukive/graphics/point.h"
#include "ukive/graphics/rect.h"


namespace ukive {

    class Geometry {
    public:
        enum class Type {
            LINE,
            RECT,
            ROUND_RECT,
            OVAL,
            PATH
        };

        virtual Type getType() = 0;
    };

    class LineGeometry : public Geometry {
    public:
        PointF start;
        PointF end;

        static LineGeometry* create(const PointF& start, const PointF& end);

        Type getType() override;

    private:
        LineGeometry() = default;
    };

    class RectGeometry : public Geometry {
    public:
        RectF rect;

        static RectGeometry* create(const RectF& rect);

        Type getType() override;

    private:
        RectGeometry() = default;
    };

    class RoundRectGeometry : public Geometry {
    public:
        RectF rect;
        float rx, ry;

        static RoundRectGeometry* create(const RectF& rect, float rx, float ry);

        Type getType() override;

    private:
        RoundRectGeometry() = default;
    };

    class OvalGeometry : public Geometry {
    public:
        float cx, cy;
        float rx, ry;

        static OvalGeometry* create(float cx, float cy, float rx, float ry);

        Type getType() override;

    private:
        OvalGeometry() = default;
    };

    class PathGeometry : public Geometry {
    public:
        static PathGeometry* create();

        Type getType() override;

    private:
        PathGeometry() = default;
    };

}

#endif  // UKIVE_GRAPHICS_GEOMETRY_H_
