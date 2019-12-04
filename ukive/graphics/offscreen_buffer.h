#ifndef UKIVE_GRAPHICS_OFFSCREEN_BUFFER_H_
#define UKIVE_GRAPHICS_OFFSCREEN_BUFFER_H_

#include <memory>

#include "ukive/graphics/cyro_buffer.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/bitmap.h"


namespace ukive {

    class OffscreenBuffer : public CyroBuffer {
    public:
        OffscreenBuffer(int width, int height);

        bool onCreate(bool hw_acc) override;
        bool onResize(int width, int height) override;
        void onDestroy() override;

        void onBeginDraw() override;
        bool onEndDraw() override;

        std::shared_ptr<Bitmap> onExtractBitmap() override;

        ComPtr<ID2D1RenderTarget> getRT() const override { return rt_; }

        ComPtr<ID3D11Texture2D> getTexture();

    private:
        bool createHardwareBRT(int width, int height);

        int width_, height_;
        ComPtr<ID3D11Texture2D> d3d_tex2d_;
        ComPtr<ID2D1RenderTarget> rt_;
    };

}

#endif  // UKIVE_GRAPHICS_OFFSCREEN_BUFFER_H_