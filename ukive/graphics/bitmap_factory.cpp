#include "bitmap_factory.h"

#include "ukive/application.h"
#include "ukive/graphics/wic_manager.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    std::shared_ptr<Bitmap> BitmapFactory::create(Window *win, unsigned int width, unsigned int height) {
        auto d2d_dc = win->getRenderer()->getD2DDeviceContext();

        ComPtr<ID2D1Bitmap> d2dBitmap;
        HRESULT hr = d2d_dc->CreateBitmap(
            D2D1::SizeU(width, height),
            D2D1::BitmapProperties(D2D1::PixelFormat()),
            &d2dBitmap);
        if (SUCCEEDED(hr)) {
            return std::make_shared<Bitmap>(d2dBitmap);
        }

        return nullptr;
    }

    std::shared_ptr<Bitmap> BitmapFactory::decodeFile(Window *win, const string16 &file_name) {
        HRESULT hr;
        ComPtr<IWICBitmapSource> source;
        ComPtr<IWICBitmapSource> destFormatSource;

        auto wic_manager = Application::getWICManager();
        auto d2d_dc = win->getRenderer()->getD2DDeviceContext();

        hr = wic_manager->decodeFile(file_name, &source);
        if (SUCCEEDED(hr)) {
            hr = wic_manager->convertForD2D(source.get(), &destFormatSource);
        }

        ComPtr<ID2D1Bitmap> d2dBitmap;

        if (SUCCEEDED(hr)) {
            hr = d2d_dc->CreateBitmapFromWicBitmap(
                destFormatSource.get(), 0, &d2dBitmap);
        }

        if (SUCCEEDED(hr)) {
            return std::make_shared<Bitmap>(d2dBitmap);
        }

        return nullptr;
    }

}