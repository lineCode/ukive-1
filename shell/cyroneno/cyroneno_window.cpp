#include "shell/cyroneno/cyroneno_window.h"

#include <algorithm>

#include "ukive/application.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/bitmap_factory.h"
#include "shell/cyroneno/ray_tracer.h"


namespace {
    constexpr auto IMAGE_WIDTH = 400;
    constexpr auto IMAGE_HEIGHT = 400;
}

namespace shell {

    CyronenoWindow::CyronenoWindow() {
    }

    void CyronenoWindow::onCreate() {
        Window::onCreate();

        cyro::RayTracer ray_tracer;
        auto image = ray_tracer.rayTracer(cyro::ProjectionType::ORTHO, IMAGE_WIDTH, IMAGE_HEIGHT);

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
