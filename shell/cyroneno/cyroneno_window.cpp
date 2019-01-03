#include "shell/cyroneno/cyroneno_window.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/graphics/point.h"
#include "shell/cyroneno/ray_tracer.h"
#include "shell/cyroneno/pipeline/rasterizer.h"
#include "shell/cyroneno/pipeline/pipeline.h"
#include "ukive/log.h"

namespace {
    constexpr auto IMAGE_WIDTH = 400;
    constexpr auto IMAGE_HEIGHT = 400;
}

namespace shell {

    CyronenoWindow::CyronenoWindow() {
    }

    void CyronenoWindow::onCreate() {
        Window::onCreate();

        showTitleBar();
        setBackgroundColor(ukive::Color(0.5f, 0.5f, 0.5f));

        /*cyro::RayTracer ray_tracer;
        auto image = ray_tracer.rayTracer(cyro::ProjectionType::ORTHO, IMAGE_WIDTH, IMAGE_HEIGHT);*/

        /*cyro::Rasterizer rasterizer(IMAGE_WIDTH, IMAGE_HEIGHT);
        rasterizer.drawLine({100, 100}, {200, 99}, cyro::Color(0, 0, 0, 1));
        rasterizer.drawTriangle(
            {50, 50}, {70, 200}, {300, 50},
            cyro::Color(1, 0, 0, 1),
            cyro::Color(0, 1, 0, 1),
            cyro::Color(0, 0, 1, 1));
        auto image = rasterizer.getOutput();*/

        cyro::Pipeline pipeline(IMAGE_WIDTH, IMAGE_HEIGHT);
        pipeline.launch();
        auto image = pipeline.getOutput();

        auto img_data_ptr = reinterpret_cast<unsigned char*>(image.data_.data());

        // Save to file
        /*ukive::Application::getWICManager()->saveToPngFile(
            IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr, L"test.png");*/

        bmp_ = ukive::BitmapFactory::create(this, IMAGE_WIDTH, IMAGE_HEIGHT, img_data_ptr);
        invalidate();
    }

    void CyronenoWindow::onDrawCanvas(ukive::Canvas* canvas) {
        Window::onDrawCanvas(canvas);

        if (bmp_) {
            int x = (getClientWidth() - IMAGE_WIDTH) / 2;
            int y = (getClientHeight() - IMAGE_HEIGHT) / 2;

            canvas->drawBitmap(x, y, bmp_.get());
        }
    }

}
