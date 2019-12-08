#ifndef CYRONENO_PIPELINE_PIPELINE_H_
#define CYRONENO_PIPELINE_PIPELINE_H_

#include <array>
#include <functional>
#include <vector>

#include "../image.h"
#include "../point.hpp"
#include "../equations.h"
#include "../rasterizer/rasterizer.h"


namespace cyro {

    struct Vertex {
        Point3 pos;
    };

    struct Triangle {
        int idx[3];
        bool clipped;
    };

    struct Polygon {
        // ԭʼ��������
        std::vector<Vertex> vertices;
        // ��������任�Ͳü��Ķ�������
        std::vector<Point4> t_vertices_4d;
        // �������ݣ�����Ϊ�գ�
        std::vector<std::array<int, 3>> indices;

        // ���ü��������ε�����
        std::vector<bool> cts_;

        // �����������вü����ɵ�������
        std::vector<Point3> cgtv_3d;
        // ����Ļ�����вü����ɵ�������
        std::vector<Point4> cgtv_4d;
        std::vector<std::array<int, 3>> cgti;
    };

    class Pipeline {
    public:
        Pipeline(int image_width, int image_height);

        void launch();

        void constructCube();
        void constructSphere();

        const ImagePng* getOutput() const;

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
            double left, double right, double top, double bottom, double near, double far,
            bool is_persp);

        void clipEdge(const PlaneEqu& cp, Point3& a, Point3& b, int* which_clipped);
        bool clipTriangle(
            const PlaneEqu& cp, const Point3 p[3], const int indices[3], Polygon* polygon);

        void clipEdge(const PlaneEqu4D& cp, Point4& a, Point4& b, int* which_clipped);
        bool clipTriangle(
            const PlaneEqu4D& cp, const Point4 p[3], const int indices[3], Polygon* polygon);

        int img_width_;
        int img_height_;
        Rasterizer rasterizer_;
        std::vector<Polygon> polygons_;

        bool is_persp_;
        uint32_t flags_;
    };

}

#endif  // CYRONENO_PIPELINE_PIPELINE_H_