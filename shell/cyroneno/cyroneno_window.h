#ifndef SHELL_CYRONENO_CYRONENO_WINDOW_H_
#define SHELL_CYRONENO_CYRONENO_WINDOW_H_

#include <memory>

#include "ukive/window/window.h"


namespace ukive {
    class Bitmap;
}

namespace shell {

    class CyronenoWindow : public ukive::Window {
    public:
        CyronenoWindow();

        void onCreate() override;
        void onDrawCanvas(ukive::Canvas* canvas) override;

    private:
        std::shared_ptr<ukive::Bitmap> bmp_;
    };

}

#endif  // SHELL_CYRONENO_CYRONENO_WINDOW_H_