#ifndef CYRONENO_PIPELINE_PIPELINE_H_
#define CYRONENO_PIPELINE_PIPELINE_H_

#include <array>
#include <functional>

#include "../image.h"
#include "../point.h"
#include "../equations.h"
#include "rasterizer.h"


namespace cyro {

    struct Vertex {
        Point3 pos;
    };

    struct Polygon {
        std::vector<Vertex> vertices;
        std::vector<Point4> t_vertices_4d;
        std::vector<std::array<int, 3>> indices;
    };

    class Pipeline {
    public:
        Pipeline(int image_width, int image_height);

        void launch();

        void constructCube();
        void constructSphere();

        ImagePng getOutput() const;

    private:
        enum Flags {
            CLIP_BEFORE = 1 << 0,
            CLIP_AFTER = 1 << 1,
        };

        using TriangleHandler = std::function<void()>;

        void processVertices();
        void rasterize();

        void clipTrianglesBefore(
            double left, double right, double top, double bottom, double near, double far,
            const Vector3& eye, const Vector3& look, const Vector3& up);
        void clipTrianglesAfter(
            double left, double right, double top, double bottom, double near, double far);
        void clipTrianglesAfter2(
            double left, double right, double top, double bottom, double near, double far);

        void clipEdge(const PlaneEqu& cp, Point3& a, Point3& b);
        bool clipTriangle(const PlaneEqu& cp, Point3& p0, Point3& p1, Point3& p2);

        void clipEdge(const PlaneEqu4D& cp, Point4& a, Point4& b);
        bool clipTriangle(const PlaneEqu4D& cp, Point4& p0, Point4& p1, Point4& p2);

        int img_width_;
        int img_height_;
        Rasterizer rasterizer_;
        std::vector<Polygon> polygons_;

        bool is_persp_;
        uint32_t flags_;
    };

}

#endif  // CYRONENO_PIPELINE_PIPELINE_H_