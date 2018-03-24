#include "wic_manager.h"

#include "ukive/log.h"


namespace ukive {

    WICManager::WICManager() {
    }


    WICManager::~WICManager() {
    }


    HRESULT WICManager::init() {
        HRESULT hr;

        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            0,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wic_factory_));
        if (FAILED(hr)) {
            return hr;
        }

        return hr;
    }

    void WICManager::close() {
    }

    ComPtr<IWICBitmap> WICManager::createBitmap(unsigned int width, unsigned int height) {
        ComPtr<IWICBitmap> bmp;
        HRESULT hr = wic_factory_->CreateBitmap(
            width, height, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnLoad, &bmp);
        DCHECK(SUCCEEDED(hr));

        return bmp;
    }

    HRESULT WICManager::convertForD2D(IWICBitmapSource *src, IWICBitmapSource **dst) {
        HRESULT hr;
        IWICFormatConverter *converter = 0;

        // Format convert the frame to 32bppPBGRA
        hr = wic_factory_->CreateFormatConverter(&converter);

        if (SUCCEEDED(hr)) {
            hr = converter->Initialize(
                src,                          // Input bitmap to convert
                GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
                WICBitmapDitherTypeNone,         // Specified dither pattern
                0,                            // Specify a particular palette
                0.f,                             // Alpha threshold
                WICBitmapPaletteTypeCustom       // Palette translation type
            );
        }

        if (SUCCEEDED(hr)) {
            *dst = converter;
        }

        return hr;
    }


    HRESULT WICManager::decodeFile(const string16 &file_name, IWICBitmapSource **out_src) {
        HRESULT hr;
        ComPtr<IWICBitmapDecoder> decoder;

        hr = wic_factory_->CreateDecoderFromFilename(
            file_name.c_str(),                     // Image to be decoded
            0,                                     // Do not prefer a particular vendor
            GENERIC_READ,                          // Desired read access to the file
            WICDecodeMetadataCacheOnDemand,        // Cache metadata when needed
            &decoder);                             // Pointer to the decoder

        // Retrieve the first frame of the image from the decoder
        IWICBitmapFrameDecode *frame = nullptr;

        if (SUCCEEDED(hr)) {
            hr = decoder->GetFrame(0, &frame);
        }

        if (SUCCEEDED(hr)) {
            *out_src = frame;
        }

        return hr;
    }


    HRESULT WICManager::getBitmapFromSource(IWICBitmapSource *src, IWICBitmap **out_bitmap) {
        HRESULT hr;
        IWICBitmap *bitmap = nullptr;

        hr = wic_factory_->CreateBitmapFromSource(
            src,
            WICBitmapCacheOnDemand,
            &bitmap);

        if (SUCCEEDED(hr)) {
            *out_bitmap = bitmap;
        }

        return hr;
    }

}