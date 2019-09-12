#ifndef SHELL_CYRONENO_CYRONENO_WINDOW_H_
#define SHELL_CYRONENO_CYRONENO_WINDOW_H_

#include <memory>

#include "ukive/window/window.h"


namespace ukive {
    class Bitmap;
    class ImageView;
}

namespace shell {

    class CyronenoWindow : public ukive::Window {
    public:
        CyronenoWindow();

        void onCreate() override;

    private:
        ukive::ImageView* img_view_ = nullptr;
        std::shared_ptr<ukive::Bitmap> bmp_;
    };

}

#endif  // SHELL_CYRONENO_CYRONENO_WINDOW_H_