#include "geometry.h"


namespace ukive {

    LineGeometry* LineGeometry::create(const PointF& start, const PointF& end) {
        auto geo = new LineGeometry();
        geo->start = start;
        geo->end = end;
        return geo;
    }

    Geometry::Type LineGeometry::getType() {
        return Type::LINE;
    }


    RectGeometry* RectGeometry::create(const RectF& rect) {
        auto geo = new RectGeometry();
        geo->rect = rect;
        return geo;
    }

    Geometry::Type RectGeometry::getType() {
        return Type::RECT;
    }


    RoundRectGeometry* RoundRectGeometry::create(const RectF& rect, float rx, float ry) {
        auto geo = new RoundRectGeometry();
        geo->rect = rect;
        geo->rx = rx;
        geo->ry = ry;
        return geo;
    }

    Geometry::Type RoundRectGeometry::getType() {
        return Type::ROUND_RECT;
    }


    OvalGeometry* OvalGeometry::create(float cx, float cy, float rx, float ry) {
        auto geo = new OvalGeometry();
        geo->cx = cx;
        geo->cy = cy;
        geo->rx = rx;
        geo->ry = ry;
        return geo;
    }

    Geometry::Type OvalGeometry::getType() {
        return Type::OVAL;
    }


    PathGeometry* PathGeometry::create() {
        auto geo = new PathGeometry();
        return geo;
    }

    Geometry::Type PathGeometry::getType() {
        return Type::PATH;
    }

}