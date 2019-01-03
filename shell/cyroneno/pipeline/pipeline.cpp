#include "pipeline.h"

#include "../matrix.h"
#include "../vector.h"


namespace cyro {

    Pipeline::Pipeline(int image_width, int image_height)
        : img_width_(image_width),
          img_height_(image_height),
          rasterizer_(image_width, image_height),
          is_persp_(false),
          flags_(CLIP_AFTER) {}

    void Pipeline::launch() {
        polygons_.clear();

        constructCube();
        constructSphere();

        processVertices();

        rasterize();
    }

    void Pipeline::constructCube() {
        double hl = 50;
        Polygon cube;
        cube.vertices.push_back({ Point3(-hl, hl, hl) });
        cube.vertices.push_back({ Point3(hl, hl, hl) });
        cube.vertices.push_back({ Point3(hl, hl, -hl) });
        cube.vertices.push_back({ Point3(-hl, hl, -hl) });
        cube.vertices.push_back({ Point3(-hl, -hl, hl) });
        cube.vertices.push_back({ Point3(hl, -hl, hl) });
        cube.vertices.push_back({ Point3(hl, -hl, -hl) });
        cube.vertices.push_back({ Point3(-hl, -hl, -hl) });

        cube.indices.push_back({ 0, 4, 5 });
        cube.indices.push_back({ 0, 5, 1 });
        cube.indices.push_back({ 1, 5, 6 });
        cube.indices.push_back({ 1, 6, 2 });
        cube.indices.push_back({ 2, 6, 7 });
        cube.indices.push_back({ 2, 7, 3 });
        cube.indices.push_back({ 3, 7, 4 });
        cube.indices.push_back({ 3, 4, 0 });
        cube.indices.push_back({ 3, 0, 1 });
        cube.indices.push_back({ 3, 1, 2 });
        cube.indices.push_back({ 4, 7, 6 });
        cube.indices.push_back({ 4, 6, 5 });

        polygons_.push_back(cube);
    }

    void Pipeline::constructSphere() {

    }

    ImagePng Pipeline::getOutput() const {
        return rasterizer_.getOutput();
    }

    void Pipeline::processVertices() {
        double left = -87;
        double right = 87;
        double bottom = -87;
        double top = 87;
        double near = -86;
        double far = -87 * 3;

        Vector3 eye(100, 100, 100);
        Vector3 look(-1, -1, -1);
        Vector3 up(0, 1, 0);

        if (flags_ & Flags::CLIP_BEFORE) {
            // 在世界坐标中裁剪
            clipTrianglesBefore(
                left, right, top, bottom, near, far,
                eye, look, up);
        }

        Matrix4x4 matrix;
        if (is_persp_) {
            matrix = Matrix4x4::viewport(img_width_, img_height_) *
                Matrix4x4::orthoProj(left, right, bottom, top, near, far) *
                Matrix4x4::persp(near, far) *
                Matrix4x4::camera(eye, look, up);
        } else {
            matrix = Matrix4x4::viewport(img_width_, img_height_) *
                Matrix4x4::orthoProj(left, right, bottom, top, near, far) *
                Matrix4x4::camera(eye, look, up);
        }

        for (auto& polygon : polygons_) {
            for (auto& vertex : polygon.vertices) {
                auto p = matrix * Point4(vertex.pos, 1);
                polygon.t_vertices_4d.push_back(p);
            }
        }

        if (flags_ & Flags::CLIP_AFTER) {
            // 在 4D 屏幕空间（像素空间）中裁剪。
            // 使用透视投影时，每个点的坐标经过变换后，在除以 w 之前会被关于原点对称（每分量取相反数），
            // 因此需要使用和正交投影不同的裁剪方法，即让裁剪面也关于原点对称。
            if (is_persp_) {
                clipTrianglesAfter(0, img_width_ - 1, img_height_ - 1, 0, 1, -1);
            } else {
                clipTrianglesAfter2(0, img_width_ - 1, img_height_ - 1, 0, 1, -1);
            }
        }

        if (is_persp_) {
            for (auto& polygon : polygons_) {
                for (auto& vertex : polygon.t_vertices_4d) {
                    auto& p = vertex;
                    p.x_ /= p.w_;
                    p.y_ /= p.w_;
                    p.z_ /= p.w_;
                    p.w_ = 1;
                }
            }
        }
    }

    void Pipeline::rasterize() {
        for (const auto& polygon : polygons_) {
            if (polygon.indices.empty()) {
                int index = 0;
                Point2 tri_pts[3];
                for (const auto& p : polygon.t_vertices_4d) {
                    tri_pts[index] = Point2(p.x_, p.y_);
                    ++index;
                    if (index >= 3) {
                        index = 0;
                        rasterizer_.drawLine(tri_pts[0], tri_pts[1], Color(0, 0, 0, 1));
                        rasterizer_.drawLine(tri_pts[1], tri_pts[2], Color(0, 0, 0, 1));
                        rasterizer_.drawLine(tri_pts[2], tri_pts[0], Color(0, 0, 0, 1));
                    }
                }
            } else {
                for (const auto& index : polygon.indices) {
                    Point2 tri_pts[3];
                    for (int i = 0; i < 3; ++i) {
                        auto p = polygon.t_vertices_4d[index[i]];
                        tri_pts[i] = Point2(p.x_, p.y_);
                    }
                    rasterizer_.drawLine(tri_pts[0], tri_pts[1], Color(0, 0, 0, 1));
                    rasterizer_.drawLine(tri_pts[1], tri_pts[2], Color(0, 0, 0, 1));
                    rasterizer_.drawLine(tri_pts[2], tri_pts[0], Color(0, 0, 0, 1));
                }
            }
        }
    }

    void Pipeline::clipTrianglesBefore(
        double left, double right, double top, double bottom, double near, double far,
        const Vector3& eye, const Vector3& look, const Vector3& up)
    {
        Point3 lbn(left, bottom, near);
        Point3 rbn(right, bottom, near);
        Point3 ltn(left, top, near);
        Point3 rtn(right, top, near);
        Point3 lbf(left, bottom, far);
        Point3 rbf(right, bottom, far);
        Point3 ltf(left, top, far);
        Point3 rtf(right, top, far);

        auto cam_inv_m = Matrix4x4::cameraInverse(eye, look, up);
        lbn = (cam_inv_m * Point4(lbn, 1)).toPoint3();
        rbn = (cam_inv_m * Point4(rbn, 1)).toPoint3();
        ltn = (cam_inv_m * Point4(ltn, 1)).toPoint3();
        rtn = (cam_inv_m * Point4(rtn, 1)).toPoint3();
        lbf = (cam_inv_m * Point4(lbf, 1)).toPoint3();
        rbf = (cam_inv_m * Point4(rbf, 1)).toPoint3();
        ltf = (cam_inv_m * Point4(ltf, 1)).toPoint3();
        rtf = (cam_inv_m * Point4(rtf, 1)).toPoint3();

        // left clip plane
        auto n_left = ((lbn - ltn) ^ (lbn - lbf)).normalize();
        auto q_left = lbn;
        PlaneEqu lcp(n_left, q_left);

        // top clip plane
        auto n_top = ((ltn - rtn) ^ (ltn - ltf)).normalize();
        auto q_top = ltn;
        PlaneEqu tcp(n_top, q_top);

        // right clip plane
        auto n_right = ((rbf - rbn) ^ (rtn - rbn)).normalize();
        auto q_right = rbn;
        PlaneEqu rcp(n_right, q_right);

        // bottom clip plane
        auto n_bottom = ((rbn - lbn) ^ (rbn - rbf)).normalize();
        auto q_bottom = rbn;
        PlaneEqu bcp(n_bottom, q_bottom);

        // near clip plane
        auto n_near = ((rbn - rtn) ^ (rbn - lbn)).normalize();
        auto q_near = rbn;
        PlaneEqu ncp(n_near, q_near);

        // far clip plane
        auto n_far = ((ltf - lbf) ^ (rbf - lbf)).normalize();
        auto q_far = lbf;
        PlaneEqu fcp(n_far, q_far);

        for (auto& polygon : polygons_) {
            if (polygon.indices.empty()) {
                int index = 0;
                Point3* tri_pts[3] {};
                for (auto& v : polygon.vertices) {
                    tri_pts[index] = &v.pos;
                    ++index;
                    if (index >= 3) {
                        index = 0;

                        if (!clipTriangle(lcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(rcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(tcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(bcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(ncp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(fcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                    }
                }
            } else {
                for (auto& index : polygon.indices) {
                    Point3* tri_pts[3] {};
                    for (int i = 0; i < 3; ++i) {
                        auto& v = polygon.vertices[index[i]];
                        tri_pts[i] = &v.pos;
                    }

                    if (!clipTriangle(lcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(rcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(tcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(bcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(ncp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(fcp, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                }
            }
        }
    }

    void Pipeline::clipTrianglesAfter(
        double left, double right, double top, double bottom, double near, double far)
    {
        // left clip hyperplane
        PlaneEqu4D lch(Vector4(1, 0, 0, -left), Point4(left, 0, 0, 1));

        // right clip hyperplane
        PlaneEqu4D rch(Vector4(-1, 0, 0, right), Point4(right, 0, 0, 1));

        // bottom clip hyperplane
        PlaneEqu4D bch(Vector4(0, 1, 0, -bottom), Point4(0, bottom, 0, 1));

        // top clip hyperplane
        PlaneEqu4D tch(Vector4(0, -1, 0, top), Point4(0, top, 0, 1));

        // near clip hyperplane
        PlaneEqu4D nch(Vector4(0, 0, -1, near), Point4(0, 0, near, 1));

        // far clip hyperplane
        PlaneEqu4D fch(Vector4(0, 0, 1, -far), Point4(0, 0, far, 1));

        for (auto& polygon : polygons_) {
            if (polygon.indices.empty()) {
                int index = 0;
                Point4* tri_pts[3]{};
                for (auto& v : polygon.t_vertices_4d) {
                    tri_pts[index] = &v;
                    ++index;
                    if (index >= 3) {
                        index = 0;

                        if (!clipTriangle(lch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(rch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(tch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(bch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(nch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(fch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                    }
                }
            } else {
                for (auto& index : polygon.indices) {
                    Point4* tri_pts[3]{};
                    for (int i = 0; i < 3; ++i) {
                        auto& v = polygon.t_vertices_4d[index[i]];
                        tri_pts[i] = &v;
                    }

                    if (!clipTriangle(lch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(rch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(tch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(bch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(nch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(fch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                }
            }
        }
    }

    void Pipeline::clipTrianglesAfter2(
        double left, double right, double top, double bottom, double near, double far)
    {
        // left clip hyperplane
        PlaneEqu4D lch(Vector4(-1, 0, 0, left), Point4(left, 0, 0, 1));

        // right clip hyperplane
        PlaneEqu4D rch(Vector4(1, 0, 0, -right), Point4(right, 0, 0, 1));

        // bottom clip hyperplane
        PlaneEqu4D bch(Vector4(0, -1, 0, bottom), Point4(0, bottom, 0, 1));

        // top clip hyperplane
        PlaneEqu4D tch(Vector4(0, 1, 0, -top), Point4(0, top, 0, 1));

        // near clip hyperplane
        PlaneEqu4D nch(Vector4(0, 0, 1, -near), Point4(0, 0, near, 1));

        // far clip hyperplane
        PlaneEqu4D fch(Vector4(0, 0, -1, far), Point4(0, 0, far, 1));

        for (auto& polygon : polygons_) {
            if (polygon.indices.empty()) {
                int index = 0;
                Point4* tri_pts[3]{};
                for (auto& v : polygon.t_vertices_4d) {
                    tri_pts[index] = &v;
                    ++index;
                    if (index >= 3) {
                        index = 0;

                        if (!clipTriangle(lch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(rch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(tch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(bch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(nch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                        if (!clipTriangle(fch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                            continue;
                        }
                    }
                }
            } else {
                for (auto& index : polygon.indices) {
                    Point4* tri_pts[3]{};
                    for (int i = 0; i < 3; ++i) {
                        auto& v = polygon.t_vertices_4d[index[i]];
                        tri_pts[i] = &v;
                    }

                    if (!clipTriangle(lch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(rch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(tch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(bch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(nch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                    if (!clipTriangle(fch, *tri_pts[0], *tri_pts[1], *tri_pts[2])) {
                        continue;
                    }
                }
            }
        }
    }

    void Pipeline::clipEdge(const PlaneEqu& cp, Point3& a, Point3& b) {
        auto fa = cp.cal(a);
        auto fb = cp.cal(b);
        if (fa * fb < 0) {
            auto t = (cp.n_*a.toVector() + cp.d_) / (cp.n_*(a - b));
            auto p = a + (b - a)*t;
            if (fa > 0) {
                a = p;
            } else {
                b = p;
            }
        }
    }

    bool Pipeline::clipTriangle(const PlaneEqu& cp, Point3& p0, Point3& p1, Point3& p2) {
        if (cp.cal(p0) > 0 && cp.cal(p1) > 0 && cp.cal(p2) > 0) {
            p0 = {};
            p1 = {};
            p2 = {};
            return false;
        }
        clipEdge(cp, p0, p1);
        clipEdge(cp, p0, p2);
        return true;
    }

    void Pipeline::clipEdge(const PlaneEqu4D& cp, Point4& a, Point4& b) {
        auto fa = cp.cal(a);
        auto fb = cp.cal(b);
        if (fa * fb < 0) {
            auto t = (cp.n_*a.toVector() + cp.d_) / (cp.n_*(a - b));
            auto p = a + (b - a)*t;
            if (fa > 0) {
                a = p;
            } else {
                b = p;
            }
        }
    }

    bool Pipeline::clipTriangle(const PlaneEqu4D& cp, Point4& p0, Point4& p1, Point4& p2) {
        // 当点位于法线指向的一边时，就认为该点位于裁剪面之外，需要处理
        if (cp.cal(p0) > 0 && cp.cal(p1) > 0 && cp.cal(p2) > 0) {
            p0 = {};
            p1 = {};
            p2 = {};
            return false;
        }
        clipEdge(cp, p0, p1);
        clipEdge(cp, p0, p2);
        return true;
    }

}