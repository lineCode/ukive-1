#include "bitmap_factory.h"

#include "ukive/application.h"
#include "ukive/graphics/wic_manager.h"


namespace ukive {

    BitmapFactory::BitmapFactory(ComPtr<ID2D1DeviceContext> d2d_dc)
        :d2d_dc_(d2d_dc) {
        wic_manager_ = Application::getWICManager();
    }


    BitmapFactory::~BitmapFactory() {
    }


    ComPtr<ID2D1Bitmap> BitmapFactory::decodeFile(const string16 &file_name) {
        HRESULT hr;
        ComPtr<IWICBitmapSource> source;
        ComPtr<IWICBitmapSource> destFormatSource;

        hr = wic_manager_->decodeFile(file_name, &source);
        if (SUCCEEDED(hr)) {
            hr = wic_manager_->convertForD2D(source.get(), &destFormatSource);
        }

        ComPtr<ID2D1Bitmap1> d2dBitmap;

        if (SUCCEEDED(hr)) {
            hr = d2d_dc_->CreateBitmapFromWicBitmap(
                destFormatSource.get(), 0, &d2dBitmap);
        }

        if (SUCCEEDED(hr)) {
            return d2dBitmap.cast<ID2D1Bitmap>();
        }

        return nullptr;
    }

}