#include "ray_tracer.h"

#include <algorithm>

#include "ukive/message/message.h"

#include "surface.h"


namespace cyro {

    RayTracer::RayTracer()
        : need_stop_(false)
    {
        cycler_.setListener(this);

        initLights();
        initSurfaces();
    }

    RayTracer::~RayTracer() {
    }

    void RayTracer::rayTracer(
        ProjectionType type, int img_width, int img_height, ImagePng* image)
    {
        double l = -img_width;
        double r = img_width;
        double t = img_height;
        double b = -img_height;

        Point3 eye(0, 100, 0);
        Vector3 view_dir(0, -0.3, -1);
        Vector3 up_vector(0, 1, 0);

        Vector3 cb_w = -view_dir.normalize();
        Vector3 cb_u = (up_vector ^ cb_w).normalize();
        Vector3 cb_v = cb_w ^ cb_u;

        for (int j = 0; j < img_height; ++j) {
            for (int i = 0; i < img_width; ++i) {
                double u = l + (r - l)*(i + 0.5) / img_width;
                double v = b + (t - b)*(j + 0.5) / img_height;

                Ray ray;
                if (type == ORTHO) {
                    ray.direction = -cb_w;
                    ray.origin = eye + cb_u * u + cb_v * v;
                } else if (type == PERSP) {
                    float d = 500;
                    ray.direction = -cb_w * d + cb_u * u + cb_v * v;
                    ray.origin = eye;
                }

                auto color = rayColor(ray, 0, std::numeric_limits<double>::max());
                image->setColor(i, j, color.toBGRAInt(255));
            }
        }
    }

    void RayTracer::rayTracerAsync(
        ProjectionType type, int img_width, int img_height, RayTracerListener* listener)
    {
        listener_ = listener;
        worker_ = std::thread(&RayTracer::run, this, type, img_width, img_height);
    }

    void RayTracer::stop() {
        need_stop_ = true;
        worker_.join();
        cycler_.setListener(nullptr);
    }

    void RayTracer::onHandleMessage(ukive::Message* msg) {
        switch (msg->what) {
        case 10089:
            if (listener_) {
                auto info = static_cast<std::vector<RenderInfo>*>(msg->shared_data.get());
                listener_->onPixelData(*info);
            }
            break;
        default:
            break;
        }
    }

    void RayTracer::initLights() {
        Light light1;
        light1.pos = Point3(-300, 300, -800);
        light1.intensity = ColorRGB(0.4f, 0.4f, 0.4f);
        lights_.push_back(light1);

        Light light2;
        light2.pos = Point3(100, 100, 0);
        light2.intensity = ColorRGB(0.2f, 0.2f, 0.2f);
        lights_.push_back(light2);
    }

    void RayTracer::initSurfaces() {
        auto sphere1 = new Sphere(Point3(-100, 82, -600), 80);
        sphere1->setKd(ColorBGRInt(100, 100, 100));
        sphere1->setKs(ColorBGRInt(205, 205, 205));
        sphere1->setPhongExp(80);
        sphere1->setShadingMethod(Surface::DIFFUSE | Surface::SPECULAR);
        scene_.addSurface(std::shared_ptr<Surface>(sphere1));

        auto sphere2 = new Sphere(Point3(100, 82, -600), 80);
        sphere2->setKd(ColorBGRInt(220, 220, 220));
        sphere2->setKs(ColorBGRInt(205, 205, 205));
        sphere2->setPhongExp(80);
        sphere2->setShadingMethod(Surface::DIFFUSE | Surface::SPECULAR);
        scene_.addSurface(std::shared_ptr<Surface>(sphere2));

        double edge_length = 1000;
        auto plane1 = new Plane(
            { Point3(edge_length, 0, 0),
              Point3(-edge_length, 0, 0),
              Point3(-edge_length, 0, -edge_length - 1000),
              Point3(edge_length, 0, -edge_length - 1000) },
            Vector3(0, 1, 0));
        plane1->setKd(ColorBGRInt(140, 140, 140));
        plane1->setShadingMethod(Surface::DIFFUSE);
        scene_.addSurface(std::shared_ptr<Surface>(plane1));
    }

    ColorBGRInt RayTracer::rayColor(const Ray& ray, double t0, double t1) {
        float ambient_intensity = 0.5f;
        ColorBGRInt ambient_color(64, 64, 64);
        ColorBGRInt background_color(32, 32, 32);

        HitRecord hit;
        if (scene_.hit(ray, t0, t1, hit)) {
            auto color = ambient_color * ambient_intensity;

            {
                // 镜面反射（可能反射多次）
                auto vv = (ray.origin - hit.p).normalize();
                auto re = (-vv + hit.n*(2 * (vv*hit.n))).normalize();

                Ray re_ray;
                re_ray.origin = hit.p;
                re_ray.direction = re;

                color += rayColor(re_ray, 0.001, std::numeric_limits<double>::max()) * 0.5f;
            }

            for (const auto& light : lights_) {
                auto lv = (light.pos - hit.p).normalize();
                auto vv = (ray.origin - hit.p).normalize();
                auto hv = (vv + lv).normalize();

                Ray shadow_ray;
                shadow_ray.direction = lv;
                shadow_ray.origin = hit.p;

                HitRecord shadow_hit;
                if (!scene_.hit(shadow_ray, 0.001, std::numeric_limits<double>::max(), shadow_hit)) {
                    auto cur_surface = scene_.getSurface(hit.index);
                    if (cur_surface->getShadingMethod() & Surface::DIFFUSE) {
                        color += cur_surface->getKd() * light.intensity * std::max(0.f, float(hit.n*lv));
                    }
                    if (cur_surface->getShadingMethod() & Surface::SPECULAR) {
                        color += cur_surface->getKs() * light.intensity
                            * float(std::pow(std::max(0., hit.n*hv), cur_surface->getPhongExp()));
                    }
                }
            }

            return color;
        }

        return background_color;
    }

    void RayTracer::run(ProjectionType type, int img_width, int img_height) {
        double l = -img_width;
        double r = img_width;
        double t = img_height;
        double b = -img_height;

        Point3 eye(0, 100, 0);
        Vector3 view_dir(0, -0.3, -1);
        Vector3 up_vector(0, 1, 0);

        Vector3 cb_w = -view_dir.normalize();
        Vector3 cb_u = (up_vector ^ cb_w).normalize();
        Vector3 cb_v = cb_w ^ cb_u;

        for (int j = 0; j < img_height; ++j) {
            auto ifs = std::make_shared<std::vector<RenderInfo>>();
            for (int i = 0; i < img_width; ++i) {
                if (need_stop_) {
                    return;
                }

                double u = l + (r - l)*(i + 0.5) / img_width;
                double v = b + (t - b)*(j + 0.5) / img_height;

                Ray ray;
                if (type == ORTHO) {
                    ray.direction = -cb_w;
                    ray.origin = eye + cb_u * u + cb_v * v;
                } else if (type == PERSP) {
                    float d = 500;
                    ray.direction = -cb_w * d + cb_u * u + cb_v * v;
                    ray.origin = eye;
                }

                auto color = rayColor(ray, 0, std::numeric_limits<double>::max());

                RenderInfo info;
                info.x = i;
                info.y = j;
                info.color = color.toBGRAInt(255);
                ifs->push_back(info);
            }

            auto msg = ukive::Message::obtain();
            msg->what = 10089;
            msg->shared_data = ifs;
            cycler_.sendMessageDelayed(msg, 10);
        }
    }

}