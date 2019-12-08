#include "bitmap_factory.h"

#include "utils/log.h"

#include "ukive/application.h"
#include "ukive/window/window.h"
#include "ukive/graphics/canvas.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    SWBitmap BitmapFactory::create(unsigned int width, unsigned int height) {
        auto wic_factory = Application::getGraphicDeviceManager()->getWICFactory();

        ComPtr<IWICBitmap> bmp;
        HRESULT hr = wic_factory->CreateBitmap(
            width, height, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnLoad, &bmp);
        DCHECK(SUCCEEDED(hr));

        return SWBitmap(bmp);
    }

    std::shared_ptr<Bitmap> BitmapFactory::create(
        Window* win, unsigned int width, unsigned int height)
    {
        auto d2d_rt = win->getCanvas()->getRT();

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
        auto d2d_rt = win->getCanvas()->getRT();

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
        auto d2d_rt = win->getCanvas()->getRT();

        auto bitmaps = decodeFile(file_name);
        if (!bitmaps.empty()) {
            ComPtr<ID2D1Bitmap> d2d_bmp;
            HRESULT hr = d2d_rt->CreateBitmapFromWicBitmap(
                bitmaps.front().getNative().get(), nullptr, &d2d_bmp);
            if (SUCCEEDED(hr)) {
                return std::make_shared<Bitmap>(d2d_bmp);
            }
        }

        return {};
    }

    SWBitmaps BitmapFactory::decodeFile(const string16& file_name) {
        auto wic_factory = Application::getGraphicDeviceManager()->getWICFactory();

        ComPtr<IWICBitmapDecoder> decoder;
        HRESULT hr = wic_factory->CreateDecoderFromFilename(
            file_name.c_str(),                     // Image to be decoded
            nullptr,                               // Do not prefer a particular vendor
            GENERIC_READ,                          // Desired read access to the file
            WICDecodeMetadataCacheOnLoad,          // Cache metadata on load
            &decoder);
        if (FAILED(hr)) {
            DLOG(Log::WARNING) << "Failed to decode file: " << file_name << " " << std::hex << hr;
            return {};
        }

        return ProcessDecoder(decoder);
    }

    SWBitmaps BitmapFactory::decodeMemory(const BYTE* buffer, DWORD size) {
        auto wic_factory = Application::getGraphicDeviceManager()->getWICFactory();

        ComPtr<IWICStream> stream;
        HRESULT hr = wic_factory->CreateStream(&stream);
        if (FAILED(hr)) {
            DCHECK(false);
            return {};
        }

        hr = stream->InitializeFromMemory(const_cast<BYTE*>(buffer), size);
        if (FAILED(hr)) {
            DCHECK(false);
            return {};
        }

        ComPtr<IWICBitmapDecoder> decoder;
        hr = wic_factory->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
        if (FAILED(hr)) {
            DCHECK(false);
            return {};
        }

        return ProcessDecoder(decoder);
    }

    bool BitmapFactory::saveToPngFile(UINT width, UINT height, BYTE* bgra_buf, const string16& name) {
        auto wic_factory = Application::getGraphicDeviceManager()->getWICFactory();

        ComPtr<IWICStream> stream;
        HRESULT hr = wic_factory->CreateStream(&stream);
        if (FAILED(hr)) {
            return false;
        }

        hr = stream->InitializeFromFilename(name.c_str(), GENERIC_WRITE);
        if (FAILED(hr)) {
            return false;
        }

        ComPtr<IWICBitmapEncoder> encoder;
        hr = wic_factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
        if (FAILED(hr)) {
            return false;
        }

        hr = encoder->Initialize(stream.get(), WICBitmapEncoderNoCache);
        if (FAILED(hr)) {
            return false;
        }

        ComPtr<IWICBitmapFrameEncode> frame;
        hr = encoder->CreateNewFrame(&frame, nullptr);
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->Initialize(nullptr);
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->SetSize(width, height);
        if (FAILED(hr)) {
            return false;
        }

        WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
        hr = frame->SetPixelFormat(&format);
        if (FAILED(hr)) {
            return false;
        }

        hr = IsEqualGUID(format, GUID_WICPixelFormat32bppBGRA) ? S_OK : E_FAIL;
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->WritePixels(height, width * 4, width*height * 4, bgra_buf);
        if (FAILED(hr)) {
            return false;
        }

        hr = frame->Commit();
        if (FAILED(hr)) {
            return false;
        }

        hr = encoder->Commit();
        if (FAILED(hr)) {
            return false;
        }

        return true;
    }


    void BitmapFactory::GetGlobalMetadata(
        ComPtr<IWICBitmapDecoder> decoder, SWBitmaps& bmps)
    {
        auto wic_factory = Application::getGraphicDeviceManager()->getWICFactory();

        ComPtr<IWICMetadataQueryReader> reader;
        HRESULT hr = decoder->GetMetadataQueryReader(&reader);
        if (SUCCEEDED(hr)) {
            PROPVARIANT prop_var;
            PropVariantInit(&prop_var);
            hr = reader->GetMetadataByName(L"/logscrdesc/Width", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                bmps.width = prop_var.uiVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/logscrdesc/Height", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                bmps.height = prop_var.uiVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/logscrdesc/GlobalColorTableFlag", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_BOOL && prop_var.boolVal) {
                PropVariantClear(&prop_var);
                hr = reader->GetMetadataByName(L"/logscrdesc/BackgroundColorIndex", &prop_var);
                if (SUCCEEDED(hr) && prop_var.vt == VT_UI1) {
                    UINT bg_index = prop_var.bVal;
                    ComPtr<IWICPalette> palette;
                    hr = wic_factory->CreatePalette(&palette);
                    if (SUCCEEDED(hr)) {
                        hr = decoder->CopyPalette(palette.get());
                    }

                    if (SUCCEEDED(hr)) {
                        UINT color_count = 0;
                        hr = palette->GetColorCount(&color_count);
                        if (SUCCEEDED(hr) && color_count > 0) {
                            UINT actual_count = 0;
                            WICColor* color_table = new WICColor[color_count];
                            hr = palette->GetColors(color_count, color_table, &actual_count);
                            if (SUCCEEDED(hr) && actual_count > 0 && bg_index < actual_count) {
                                bmps.bg_color = Color::ofARGB(color_table[bg_index]);
                            }
                            delete[] color_table;
                        }
                    }
                }
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/logscrdesc/PixelAspectRatio", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI1) {
                UINT ratio = prop_var.bVal;
                if (ratio != 0) {
                    float pixel_ratio = (ratio + 15.f) / 64.f;
                    if (pixel_ratio > 1.f) {
                        bmps.height = static_cast<int>(bmps.height / pixel_ratio);
                    } else {
                        bmps.width = static_cast<int>(bmps.width * pixel_ratio);
                    }
                }
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/appext/application", &prop_var);
            if (SUCCEEDED(hr)
                && prop_var.vt == (VT_UI1 | VT_VECTOR)
                && prop_var.caub.cElems == 11
                && (!memcmp(prop_var.caub.pElems, "NETSCAPE2.0", prop_var.caub.cElems) ||
                    !memcmp(prop_var.caub.pElems, "ANIMEXTS1.0", prop_var.caub.cElems))) {

                PropVariantClear(&prop_var);
                hr = reader->GetMetadataByName(L"/appext/data", &prop_var);
                if (SUCCEEDED(hr)
                    && (prop_var.vt == (VT_UI1 | VT_VECTOR)
                        && prop_var.caub.cElems >= 4
                        && prop_var.caub.pElems[0] > 0
                        && prop_var.caub.pElems[1] == 1)) {
                    bmps.loop_count = MAKEWORD(prop_var.caub.pElems[2],
                        prop_var.caub.pElems[3]);
                }
            }
            PropVariantClear(&prop_var);
        }
    }

    void BitmapFactory::GetFrameMetadata(
        ComPtr<IWICBitmapFrameDecode> decoder, GifFrame& frame)
    {
        ComPtr<IWICMetadataQueryReader> reader;
        HRESULT hr = decoder->GetMetadataQueryReader(&reader);
        if (SUCCEEDED(hr)) {
            PROPVARIANT prop_var;
            PropVariantInit(&prop_var);
            hr = reader->GetMetadataByName(L"/grctlext/Disposal", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI1) {
                frame.disposal = prop_var.bVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/grctlext/Delay", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                frame.frame_interval = prop_var.uiVal * 10;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/imgdesc/Left", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                frame.left = prop_var.uiVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/imgdesc/Top", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                frame.top = prop_var.uiVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/imgdesc/Width", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                frame.width = prop_var.uiVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/imgdesc/Height", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_UI2) {
                frame.height = prop_var.uiVal;
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/imgdesc/InterlaceFlag", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_BOOL) {
                auto v_interlace = prop_var.boolVal;
                if (v_interlace) {
                    frame.interlace = true;
                } else {
                    frame.interlace = false;
                }
            }

            PropVariantClear(&prop_var);
            hr = reader->GetMetadataByName(L"/imgdesc/SortFlag", &prop_var);
            if (SUCCEEDED(hr) && prop_var.vt == VT_BOOL) {
                auto v_sort = prop_var.boolVal;
                if (v_sort) {
                    frame.sort = true;
                } else {
                    frame.sort = false;
                }
            }
            PropVariantClear(&prop_var);
        }
    }

    SWBitmaps BitmapFactory::ProcessDecoder(ComPtr<IWICBitmapDecoder> decoder) {
        auto wic_factory = Application::getGraphicDeviceManager()->getWICFactory();

        UINT frame_count = 0;
        HRESULT hr = decoder->GetFrameCount(&frame_count);
        if (FAILED(hr) || frame_count < 1) {
            DCHECK(false);
            return {};
        }

        SWBitmaps bmps;
        GetGlobalMetadata(decoder, bmps);

        for (UINT i = 0; i < frame_count; ++i) {
            ComPtr<IWICBitmapFrameDecode> frame_decoder;
            hr = decoder->GetFrame(i, &frame_decoder);
            if (FAILED(hr)) {
                DCHECK(false);
                continue;
            }

            ComPtr<IWICFormatConverter> converter;
            // Format convert the frame to 32bppPBGRA
            hr = wic_factory->CreateFormatConverter(&converter);
            if (FAILED(hr)) {
                DCHECK(false);
                continue;
            }

            auto source = frame_decoder.cast<IWICBitmapSource>();
            if (source == nullptr) {
                DCHECK(false);
                continue;
            }

            hr = converter->Initialize(
                source.get(),                          // Input bitmap to convert
                GUID_WICPixelFormat32bppPBGRA,         // Destination pixel format
                WICBitmapDitherTypeNone,               // Specified dither pattern
                nullptr,                               // Specify a particular palette
                0.f,                                   // Alpha threshold
                WICBitmapPaletteTypeCustom             // Palette translation type
            );
            if (FAILED(hr)) {
                DCHECK(false);
                continue;
            }

            auto wic_bitmap_src = converter.cast<IWICBitmapSource>();
            if (!wic_bitmap_src) {
                DCHECK(false);
                continue;
            }

            ComPtr<IWICBitmap> wic_bitmap;
            hr = wic_factory->CreateBitmapFromSource(
                wic_bitmap_src.get(), WICBitmapCacheOnDemand, &wic_bitmap);
            if (FAILED(hr)) {
                DCHECK(false);
                continue;
            }

            GifFrame wic_frame;
            GetFrameMetadata(frame_decoder, wic_frame);
            wic_frame.bitmap = SWBitmap(wic_bitmap);

            bmps.frames.push_back(wic_frame);
        }

        if (bmps.width == 0 || bmps.height == 0) {
            if (!bmps.frames.empty()) {
                UINT width = 0;
                UINT height = 0;
                hr = bmps.frames.front().bitmap.getNative()->GetSize(&width, &height);
                if (SUCCEEDED(hr)) {
                    bmps.width = width;
                    bmps.height = height;
                }
            }
        }

        if (bmps.width == 0 || bmps.height == 0) {
            DCHECK(false);
            return {};
        }

        return bmps;
    }

}
