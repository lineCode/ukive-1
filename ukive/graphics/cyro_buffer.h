#ifndef UKIVE_GRAPHICS_CYRO_BUFFER_H_
#define UKIVE_GRAPHICS_CYRO_BUFFER_H_

#include <memory>

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class Bitmap;

    class CyroBuffer {
    public:
        virtual ~CyroBuffer() = default;

        virtual bool onCreate(bool hw_acc) = 0;
        virtual bool onResize(int width, int height) = 0;
        virtual void onDestroy() = 0;

        virtual void onBeginDraw() = 0;
        virtual bool onEndDraw() = 0;

        virtual std::shared_ptr<Bitmap> onExtractBitmap() = 0;

        // ¡Ÿ ±∑Ω∑®
        virtual ComPtr<ID2D1RenderTarget> getRT() const = 0;
    };

}

#endif  // UKIVE_GRAPHICS_CYRO_BUFFER_H_