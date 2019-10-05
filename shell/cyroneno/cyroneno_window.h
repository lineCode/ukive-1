#ifndef SHELL_CYRONENO_CYRONENO_WINDOW_H_
#define SHELL_CYRONENO_CYRONENO_WINDOW_H_

#include <memory>

#include "ukive/window/window.h"


namespace ukive {
    class Bitmap;
    class ImageView;
}

namespace cyro {
    class Rasterizer;
}

namespace shell {

    class CyronenoWindow : public ukive::Window {
    public:
        CyronenoWindow();

        void onCreate() override;

    private:
        void TEST_LINES(cyro::Rasterizer& rz);
        void TEST_ELLIPSES(cyro::Rasterizer& rz);
        void TEST_QUAD_BEZIERS(cyro::Rasterizer& rz);
        void TEST_CUBIC_BEZIERS(cyro::Rasterizer& rz);

        ukive::ImageView* img_view_ = nullptr;
        std::shared_ptr<ukive::Bitmap> bmp_;
    };

}

#endif  // SHELL_CYRONENO_CYRONENO_WINDOW_H_