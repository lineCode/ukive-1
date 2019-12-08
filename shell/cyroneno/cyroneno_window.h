#ifndef SHELL_CYRONENO_CYRONENO_WINDOW_H_
#define SHELL_CYRONENO_CYRONENO_WINDOW_H_

#include <memory>

#include "ukive/window/window.h"

#include "cyroneno/ray_tracer/ray_tracer.h"


namespace ukive {
    class Bitmap;
    class ImageView;
}

namespace cyro {
    class Rasterizer;
}

namespace shell {

    class CyronenoWindow :
        public ukive::Window,
        public cyro::RayTracerListener {
    public:
        CyronenoWindow();

        void onCreate() override;
        void onDestroy() override;

    protected:
        // cyro::RayTracerListener
        void onPixelData(const std::vector<cyro::RenderInfo>& row) override;

    private:
        void TEST_LINES(cyro::Rasterizer& rz);
        void TEST_ELLIPSES(cyro::Rasterizer& rz);
        void TEST_QUAD_BEZIERS(cyro::Rasterizer& rz);
        void TEST_CUBIC_BEZIERS(cyro::Rasterizer& rz);

        ukive::ImageView* img_view_ = nullptr;
        std::shared_ptr<ukive::Bitmap> bmp_;

        cyro::RayTracer ray_tracer_;
        std::unique_ptr<cyro::ImagePng> image_;
    };

}

#endif  // SHELL_CYRONENO_CYRONENO_WINDOW_H_