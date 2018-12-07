#include "bitmap_factory.h"

#include "ukive/application.h"
#include "ukive/graphics/wic_manager.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    std::shared_ptr<Bitmap> BitmapFactory::create(Window* win, unsigned int width, unsigned int height) {
        auto d2d_rt = win->getRenderer()->getRenderTarget();

        auto prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = d2d_rt->CreateBitmap(D2D1::SizeU(width, height), prop, &d2d_bmp);
        if (SUCCEEDED(hr)) {
            return std::make_shared<Bitmap>(d2d_bmp);
        }

        return {};
    }

    std::shared_ptr<Bitmap> BitmapFactory::create(
        Window *win, unsigned int width, unsigned int height, const void* data)
    {
        auto d2d_rt = win->getRenderer()->getRenderTarget();

        auto prop = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        ComPtr<ID2D1Bitmap> d2d_bmp;
        HRESULT hr = d2d_rt->CreateBitmap(D2D1::SizeU(width, height), data, width*4, prop, &d2d_bmp);
        if (SUCCEEDED(hr)) {
            return std::make_shared<Bitmap>(d2d_bmp);
        }

        return {};
    }

    std::shared_ptr<Bitmap> BitmapFactory::decodeFile(Window* win, const string16& file_name) {
        auto wic_manager = Application::getWICManager();
        auto d2d_rt = win->getRenderer()->getRenderTarget();

        auto bitmaps = wic_manager->decodeFile(file_name);
        if (!bitmaps.empty()) {
            ComPtr<ID2D1Bitmap> d2d_bmp;
            HRESULT hr = d2d_rt->CreateBitmapFromWicBitmap(
                bitmaps.front().get(), nullptr, &d2d_bmp);
            if (SUCCEEDED(hr)) {
                return std::make_shared<Bitmap>(d2d_bmp);
            }
        }

        return {};
    }

}