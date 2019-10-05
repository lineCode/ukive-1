#include "pipeline.h"

#include "ukive/log.h"

#include "../matrix.h"
#include "../vector.h"

#define PI 3.1415926535898


namespace cyro {

    Pipeline::Pipeline(int image_width, int image_height)
        : img_width_(image_width),
          img_height_(image_height),
          rasterizer_(image_width, image_height),
          is_persp_(false),
          flags_(CLIP_BEFORE) {}

    void Pipeline::launch() {
        polygons_.clear();

        //constructCube();
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
        double r = 80;
        int hori_div = 16;
        int vert_div = 16;
        double h_angle = 2 * PI / hori_div;
        double v_angle = PI / vert_div;

        Polygon sphere;
        for (int i = 0; i <= vert_div; ++i) {
            if (i == 0) {
                sphere.vertices.push_back({ Point3(0, r, 0) });
                for (int j = 0; j < hori_div; ++j) {
                    sphere.indices.push_back({ 0, 1 + j, (1 + j) % hori_div + 1 });
                }
            } else if (i == vert_div) {
                sphere.vertices.push_back({ Point3(0, -r, 0) });
            } else {
                double a = i * v_angle;
                int start_idx = 1 + (i - 1) * hori_div;
                int start_next_idx = 1 + i * hori_div;
                for (int j = 0; j < hori_div; ++j) {
                    double b = j * h_angle;
                    sphere.vertices.push_back({
                        Point3(r*std::sin(a)*std::cos(b), r*std::cos(a), r*std::sin(a)*std::sin(b)) });

                    int cur_idx = start_idx + j;
                    auto cur_1 = (cur_idx + 1 - start_idx) % hori_div + start_idx;
                    if (i < vert_div - 1) {
                        int nxt_idx = 1 + i * hori_div + j;
                        auto next_1 = (nxt_idx + 1 - start_next_idx) % hori_div + start_next_idx;
                        sphere.indices.push_back(
                            { cur_idx, nxt_idx, next_1 });
                        sphere.indices.push_back(
                            { cur_idx, next_1, cur_1 });
                    } else {
                        int nxt_idx = 1 + i * hori_div;
                        sphere.indices.push_back(
                            { cur_idx, nxt_idx, cur_1 });
                    }
                }
            }
        }

        polygons_.push_back(sphere);
    }

    const ImagePng* Pipeline::getOutput() const {
        return rasterizer_.getOutput();
    }

    void Pipeline::processVertices() {
        double left = -87;
        double right = 87;
        double bottom = -87;
        double top = 87;
        double near = -96;
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

            for (auto& vertex : polygon.cgtv_3d) {
                auto p = matrix * Point4(vertex, 1);
                polygon.cgtv_4d.push_back(p);
            }
        }

        if (flags_ & Flags::CLIP_AFTER) {
            clipTrianglesAfter(0, img_width_ - 1, img_height_ - 1, 0, 1, -1, is_persp_);
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

                for (auto& vertex : polygon.cgtv_4d) {
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
                // 无索引，每三个顶点绘制一个三角形
                int index = 0;
                int ct_index = 0;
                Point2 tri_pts[3];
                for (const auto& p : polygon.t_vertices_4d) {
                    tri_pts[index] = Point2(p.x_, p.y_);
                    ++index;
                    if (index >= 3) {
                        index = 0;
                        if (polygon.cts_[ct_index]) {
                            rasterizer_.drawLine(tri_pts[0], tri_pts[1], Color(0, 0, 0, 1));
                            rasterizer_.drawLine(tri_pts[1], tri_pts[2], Color(0, 0, 0, 1));
                            rasterizer_.drawLine(tri_pts[2], tri_pts[0], Color(0, 0, 0, 1));
                        }
                        ++ct_index;
                    }
                }

                index = 0;
                for (const auto& p : polygon.cgtv_4d) {
                    tri_pts[index] = Point2(p.x_, p.y_);
                    ++index;
                    if (index >= 3) {
                        index = 0;
                        rasterizer_.drawLine(tri_pts[0], tri_pts[1], Color(0, 0, 0, 1));
                        rasterizer_.drawLine(tri_pts[1], tri_pts[2], Color(0, 0, 0, 1));
                        rasterizer_.drawLine(tri_pts[2], tri_pts[0], Color(0, 0, 0, 1));
                        ++ct_index;
                    }
                }
            } else {
                // 有索引，按照索引绘制
                int ct_index = 0;
                for (const auto& index : polygon.indices) {
                    if (polygon.cts_[ct_index]) {
                        Point2 tri_pts[3];
                        for (int i = 0; i < 3; ++i) {
                            auto p = polygon.t_vertices_4d[index[i]];
                            tri_pts[i] = Point2(p.x_, p.y_);
                        }
                        rasterizer_.drawLine(tri_pts[0], tri_pts[1], Color(0, 0, 0, 1));
                        rasterizer_.drawLine(tri_pts[1], tri_pts[2], Color(0, 0, 0, 1));
                        rasterizer_.drawLine(tri_pts[2], tri_pts[0], Color(0, 0, 0, 1));
                    }
                    ++ct_index;
                }

                int rv_size = polygon.t_vertices_4d.size();
                for (const auto& index : polygon.cgti) {
                    Point2 tri_pts[3];
                    for (int i = 0; i < 3; ++i) {
                        auto idx = index[i];
                        Point4 p;
                        if (idx >= rv_size) {
                            p = polygon.cgtv_4d[idx - rv_size];
                        } else {
                            p = polygon.t_vertices_4d[idx];
                        }
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
                int tv_index = 0;
                Point3 tri_pts[3];
                for (auto& v : polygon.vertices) {
                    tri_pts[tv_index] = v.pos;
                    ++tv_index;
                    if (tv_index >= 3) {
                        tv_index = 0;

                        if (!clipTriangle(lcp, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(rcp, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(tcp, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(bcp, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(ncp, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(fcp, tri_pts, nullptr, &polygon))
                        {
                            polygon.cts_.push_back(false);
                        } else {
                            polygon.cts_.push_back(true);
                        }
                    }
                }
            } else {
                for (auto& index : polygon.indices) {
                    Point3 tri_pts[3];
                    for (int i = 0; i < 3; ++i) {
                        auto& v = polygon.vertices[index[i]];
                        tri_pts[i] = v.pos;
                    }

                    if (!clipTriangle(lcp, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(rcp, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(tcp, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(bcp, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(ncp, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(fcp, tri_pts, index.data(), &polygon))
                    {
                        polygon.cts_.push_back(false);
                    } else {
                        polygon.cts_.push_back(true);
                    }
                }
            }
        }
    }

    void Pipeline::clipTrianglesAfter(
        double left, double right, double top, double bottom, double near, double far,
        bool is_persp)
    {
        // 在 4D 屏幕空间（像素空间）中裁剪。
        // 使用透视投影时，每个点的坐标经过变换后，在除以 w 之前会被关于原点对称（每分量取相反数），
        // 因此需要使用和正交投影不同的裁剪方法，即让裁剪面也关于原点对称。
        int inv = is_persp ? 1 : -1;

        PlaneEqu4D lch(Vector4(1 * inv, 0, 0, -left * inv), Point4(left, 0, 0, 1));  // left clip hyperplane
        PlaneEqu4D rch(Vector4(-1 * inv, 0, 0, right * inv), Point4(right, 0, 0, 1));  // right clip hyperplane
        PlaneEqu4D bch(Vector4(0, 1 * inv, 0, -bottom * inv), Point4(0, bottom, 0, 1));  // bottom clip hyperplane
        PlaneEqu4D tch(Vector4(0, -1 * inv, 0, top * inv), Point4(0, top, 0, 1));  // top clip hyperplane
        PlaneEqu4D nch(Vector4(0, 0, -1 * inv, near * inv), Point4(0, 0, near, 1));  // near clip hyperplane
        PlaneEqu4D fch(Vector4(0, 0, 1 * inv, -far * inv), Point4(0, 0, far, 1));  // far clip hyperplane

        for (auto& polygon : polygons_) {
            if (polygon.indices.empty()) {
                int index = 0;
                Point4 tri_pts[3];
                for (auto& v : polygon.t_vertices_4d) {
                    tri_pts[index] = v;
                    ++index;
                    if (index >= 3) {
                        index = 0;

                        if (!clipTriangle(lch, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(rch, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(tch, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(bch, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(nch, tri_pts, nullptr, &polygon) ||
                            !clipTriangle(fch, tri_pts, nullptr, &polygon))
                        {
                            polygon.cts_.push_back(false);
                        } else {
                            polygon.cts_.push_back(true);
                        }
                    }
                }
            } else {
                for (auto& index : polygon.indices) {
                    Point4 tri_pts[3];
                    for (int i = 0; i < 3; ++i) {
                        auto& v = polygon.t_vertices_4d[index[i]];
                        tri_pts[i] = v;
                    }

                    if (!clipTriangle(lch, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(rch, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(tch, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(bch, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(nch, tri_pts, index.data(), &polygon) ||
                        !clipTriangle(fch, tri_pts, index.data(), &polygon))
                    {
                        polygon.cts_.push_back(false);
                    } else {
                        polygon.cts_.push_back(true);
                    }
                }
            }
        }
    }

    void Pipeline::clipEdge(const PlaneEqu& cp, Point3& a, Point3& b, int* which_clipped) {
        auto fa = cp.cal(a);
        auto fb = cp.cal(b);
        if (fa * fb < 0) {
            auto t = (cp.n_*a.toVector() + cp.d_) / (cp.n_*(a - b));
            auto p = a + (b - a)*t;
            if (fa > 0) {
                a = p;
                *which_clipped = 1;
            } else {
                b = p;
                *which_clipped = 2;
            }
        } else {
            *which_clipped = 0;
        }
    }

    bool Pipeline::clipTriangle(
        const PlaneEqu& cp, const Point3 p[3], const int indices[3], Polygon* polygon)
    {
        if (cp.cal(p[0]) > 0 && cp.cal(p[1]) > 0 && cp.cal(p[2]) > 0) {
            return false;
        }

        int c01, c02, c12;
        auto p0_org = p[0];
        auto p1_org = p[1];
        auto p2_org = p[2];
        auto p0_tmp = p[0];
        auto p1_tmp = p[1];
        auto p2_tmp = p[2];
        clipEdge(cp, p0_org, p1_org, &c01);
        clipEdge(cp, p0_tmp, p2_org, &c02);
        clipEdge(cp, p1_tmp, p2_tmp, &c12);

        int n = polygon->vertices.size() + polygon->cgtv_3d.size();
        if (c01 == 2 && c12 == 1) {
            if (indices) {
                // + (0, 1, 2)
                polygon->cgtv_3d.push_back(p1_org);
                polygon->cgti.push_back({ indices[0], n, indices[2] });
                // + (1, n, 2)
                polygon->cgtv_3d.push_back(p1_tmp);
                polygon->cgti.push_back({ n, n + 1, indices[2] });
            } else {
                // + (0, 1, 2)
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p[0], p1_org, p[2] });
                // + (1, n, 2)
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p1_org, p1_tmp, p[2] });
            }
        } else if (c02 == 2 && c12 == 2) {
            if (indices) {
                // + (0, 1, 2)
                polygon->cgtv_3d.push_back(p2_org);
                polygon->cgti.push_back({ indices[0], indices[1], n });
                // + (1, n, 2)
                polygon->cgtv_3d.push_back(p2_tmp);
                polygon->cgti.push_back({ indices[1], n + 1, n });
            } else {
                // + (0, 1, 2)
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p[0], p[1], p2_org });
                // + (1, n, 2)
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p[1], p2_tmp, p2_org });
            }
        } else if (c01 == 1 && c02 == 1) {
            if (indices) {
                // + (0, 1, 2)
                polygon->cgtv_3d.push_back(p0_org);
                polygon->cgti.push_back({ n, indices[1], indices[2] });
                // + (0, 2, n)
                polygon->cgtv_3d.push_back(p0_tmp);
                polygon->cgti.push_back({ n, indices[2], n + 1 });
            } else {
                // + (0, 1, 2)
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p0_org, p[1], p[2] });
                // + (0, 2, n)
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p0_org, p[2], p0_tmp });
            }
        } else if (c01 == 1 && c12 == 2) {
            // + (0, 1, 2)
            if (indices) {
                polygon->cgtv_3d.push_back(p0_org);
                polygon->cgtv_3d.push_back(p2_tmp);
                polygon->cgti.push_back({ n, indices[1], n + 1 });
            } else {
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p0_org, p[1], p2_tmp });
            }
        } else if (c02 == 1 && c12 == 1) {
            // + (0, 1, 2)
            if (indices) {
                polygon->cgtv_3d.push_back(p0_tmp);
                polygon->cgtv_3d.push_back(p1_tmp);
                polygon->cgti.push_back({ n, n + 1, indices[2] });
            } else {
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p0_tmp, p1_tmp, p[2] });
            }
        } else if (c01 == 2 && c02 == 2) {
            // + (0, 1, 2)
            if (indices) {
                polygon->cgtv_3d.push_back(p1_org);
                polygon->cgtv_3d.push_back(p2_org);
                polygon->cgti.push_back({ indices[0], n, n + 1 });
            } else {
                polygon->cgtv_3d.insert(polygon->cgtv_3d.end(), { p[0], p1_org, p2_org });
            }
        } else {
            return true;
        }

        return false;
    }

    void Pipeline::clipEdge(const PlaneEqu4D& cp, Point4& a, Point4& b, int* which_clipped) {
        auto fa = cp.cal(a);
        auto fb = cp.cal(b);
        if (fa * fb < 0) {
            auto t = (cp.n_*a.toVector() + cp.d_) / (cp.n_*(a - b));
            auto p = a + (b - a)*t;
            if (fa > 0) {
                a = p;
                *which_clipped = 1;
            } else {
                b = p;
                *which_clipped = 2;
            }
        } else {
            *which_clipped = 0;
        }
    }

    bool Pipeline::clipTriangle(
        const PlaneEqu4D& cp, const Point4 p[3], const int indices[3], Polygon* polygon)
    {
        // 当点位于法线指向的一边时，就认为该点位于裁剪面之外，需要处理
        if (cp.cal(p[0]) > 0 && cp.cal(p[1]) > 0 && cp.cal(p[2]) > 0) {
            return false;
        }

        int c01, c02, c12;
        auto p0_org = p[0];
        auto p1_org = p[1];
        auto p2_org = p[2];
        auto p0_tmp = p[0];
        auto p1_tmp = p[1];
        auto p2_tmp = p[2];
        clipEdge(cp, p0_org, p1_org, &c01);
        clipEdge(cp, p0_tmp, p2_org, &c02);
        clipEdge(cp, p1_tmp, p2_tmp, &c12);

        int n = polygon->t_vertices_4d.size() + polygon->cgtv_4d.size();
        if (c01 == 2 && c12 == 1) {
            if (indices) {
                // + (0, 1, 2)
                polygon->cgtv_4d.push_back(p1_org);
                polygon->cgti.push_back({ indices[0], n, indices[2] });
                // + (1, n, 2)
                polygon->cgtv_4d.push_back(p1_tmp);
                polygon->cgti.push_back({ n, n + 1, indices[2] });
            } else {
                // + (0, 1, 2)
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p[0], p1_org, p[2] });
                // + (1, n, 2)
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p1_org, p1_tmp, p[2] });
            }
        } else if (c02 == 2 && c12 == 2) {
            if (indices) {
                // + (0, 1, 2)
                polygon->cgtv_4d.push_back(p2_org);
                polygon->cgti.push_back({ indices[0], indices[1], n });
                // + (1, n, 2)
                polygon->cgtv_4d.push_back(p2_tmp);
                polygon->cgti.push_back({ indices[1], n + 1, n });
            } else {
                // + (0, 1, 2)
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p[0], p[1], p2_org });
                // + (1, n, 2)
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p[1], p2_tmp, p2_org });
            }
        } else if (c01 == 1 && c02 == 1) {
            if (indices) {
                // + (0, 1, 2)
                polygon->cgtv_4d.push_back(p0_org);
                polygon->cgti.push_back({ n, indices[1], indices[2] });
                // + (0, 2, n)
                polygon->cgtv_4d.push_back(p0_tmp);
                polygon->cgti.push_back({ n, indices[2], n + 1 });
            } else {
                // + (0, 1, 2)
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p0_org, p[1], p[2] });
                // + (0, 2, n)
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p0_org, p[2], p0_tmp });
            }
        } else if (c01 == 1 && c12 == 2) {
            // + (0, 1, 2)
            if (indices) {
                polygon->cgtv_4d.push_back(p0_org);
                polygon->cgtv_4d.push_back(p2_tmp);
                polygon->cgti.push_back({ n, indices[1], n + 1 });
            } else {
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p0_org, p[1], p2_tmp });
            }
        } else if (c02 == 1 && c12 == 1) {
            // + (0, 1, 2)
            if (indices) {
                polygon->cgtv_4d.push_back(p0_tmp);
                polygon->cgtv_4d.push_back(p1_tmp);
                polygon->cgti.push_back({ n, n + 1, indices[2] });
            } else {
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p0_tmp, p1_tmp, p[2] });
            }
        } else if (c01 == 2 && c02 == 2) {
            // + (0, 1, 2)
            if (indices) {
                polygon->cgtv_4d.push_back(p1_org);
                polygon->cgtv_4d.push_back(p2_org);
                polygon->cgti.push_back({ indices[0], n, n + 1 });
            } else {
                polygon->cgtv_4d.insert(polygon->cgtv_4d.end(), { p[0], p1_org, p2_org });
            }
        } else {
            return true;
        }

        return false;
    }

}