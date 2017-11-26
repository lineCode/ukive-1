#include "bitmap_factory.h"

#include "ukive/application.h"
#include "ukive/graphics/wic_manager.h"
#include "ukive/window/window.h"
#include "ukive/graphics/renderer.h"


namespace ukive {

    ComPtr<ID2D1Bitmap> BitmapFactory::create(Window *win, unsigned int width, unsigned int height) {
        auto d2d_dc = win->getRenderer()->getD2DDeviceContext();

        ComPtr<ID2D1Bitmap> d2dBitmap;
        HRESULT hr = d2d_dc->CreateBitmap(
            D2D1::SizeU(width, height),
            D2D1::BitmapProperties(D2D1::PixelFormat()),
            &d2dBitmap);
        if (SUCCEEDED(hr)) {
            return d2dBitmap;
        }

        return nullptr;
    }

    ComPtr<ID2D1Bitmap> BitmapFactory::decodeFile(Window *win, const string16 &file_name) {
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
            return d2dBitmap.cast<ID2D1Bitmap>();
        }

        return nullptr;
    }

}