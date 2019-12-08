#ifndef CYRONENO_RAY_TRACER_RAY_TRACER_H_
#define CYRONENO_RAY_TRACER_RAY_TRACER_H_

#include <atomic>
#include <thread>

#include "ukive/message/cycler.h"

#include "../color.h"
#include "../image.h"
#include "ray_tracer_structs.h"
#include "scene.h"


namespace cyro {

    struct RenderInfo {
        int x, y;
        ColorBGRAInt color;
    };

    class RayTracerListener {
    public:
        virtual ~RayTracerListener() = default;

        virtual void onPixelData(const std::vector<RenderInfo>& row) = 0;
    };

    class RayTracer : public ukive::CyclerListener {
    public:
        RayTracer();
        ~RayTracer();

        void rayTracer(
            ProjectionType type, int img_width, int img_height, ImagePng* image);
        void rayTracerAsync(
            ProjectionType type, int img_width, int img_height, RayTracerListener* listener);

        void stop();

    protected:
        // ukive::CyclerListener
        void onHandleMessage(ukive::Message* msg) override;

    private:
        void initLights();
        void initSurfaces();
        ColorBGRInt rayColor(const Ray& ray, double t0, double t1);

        void run(ProjectionType type, int img_width, int img_height);

        Scene scene_;
        std::vector<Light> lights_;

        std::thread worker_;
        ukive::Cycler cycler_;
        std::atomic_bool need_stop_;
        RayTracerListener* listener_ = nullptr;
    };

}

#endif  // CYRONENO_RAY_TRACER_RAY_TRACER_H_