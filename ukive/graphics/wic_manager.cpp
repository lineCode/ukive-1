#include "wic_manager.h"

#include "ukive/log.h"


namespace ukive {

    WICManager::WICManager() {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory1,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wic_factory_));
        if (FAILED(hr)) {
            LOG(Log::FATAL) << "Failed to create WIC factory: " << hr;
        }
    }

    WICManager::~WICManager() {
    }


    ComPtr<IWICBitmap> WICManager::createBitmap(IWICBitmapSource* src) {
        ComPtr<IWICBitmap> bitmap;
        HRESULT hr = wic_factory_->CreateBitmapFromSource(
            src,
            WICBitmapCacheOnDemand,
            &bitmap);
        if (SUCCEEDED(hr)) {
            return bitmap;
        }

        return {};
    }

    ComPtr<IWICBitmap> WICManager::createBitmap(UINT width, UINT height) {
        ComPtr<IWICBitmap> bmp;
        HRESULT hr = wic_factory_->CreateBitmap(
            width, height, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnLoad, &bmp);
        DCHECK(SUCCEEDED(hr));

        return bmp;
    }

    ComPtr<IWICBitmap> WICManager::createBitmap(UINT width, UINT height, BYTE* bgra_buf) {
        return {};
    }

    WICBitmaps WICManager::decodeFile(const string16& file_name) {
        if (!wic_factory_) {
            return {};
        }

        ComPtr<IWICBitmapDecoder> decoder;
        HRESULT hr = wic_factory_->CreateDecoderFromFilename(
            file_name.c_str(),                     // Image to be decoded
            nullptr,                               // Do not prefer a particular vendor
            GENERIC_READ,                          // Desired read access to the file
            WICDecodeMetadataCacheOnLoad,          // Cache metadata on load
            &decoder);
        if (FAILED(hr)) {
            DCHECK(false);
            return {};
        }

        return ProcessDecoder(decoder);
    }

    WICBitmaps WICManager::decodeMemory(const BYTE* buffer, size_t size) {
        if (!wic_factory_) {
            return {};
        }

        ComPtr<IWICStream> stream;
        HRESULT hr = wic_factory_->CreateStream(&stream);
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
        hr = wic_factory_->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
        if (FAILED(hr)) {
            DCHECK(false);
            return {};
        }

        return ProcessDecoder(decoder);
    }

    bool WICManager::saveToPngFile(UINT width, UINT height, BYTE* bgra_buf, const string16& name) {
        ComPtr<IWICStream> stream;
        HRESULT hr = wic_factory_->CreateStream(&stream);
        if (FAILED(hr)) {
            return false;
        }

        hr = stream->InitializeFromFilename(name.c_str(), GENERIC_WRITE);
        if (FAILED(hr)) {
            return false;
        }

        ComPtr<IWICBitmapEncoder> encoder;
        hr = wic_factory_->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
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

        hr = frame->WritePixels(height, width*4, width*height*4, bgra_buf);
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


    void WICManager::GetGlobalMetadata(
        ComPtr<IWICBitmapDecoder> decoder,
        WICBitmaps& bmps) {

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
                    hr = wic_factory_->CreatePalette(&palette);
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

    void WICManager::GetFrameMetadata(
        ComPtr<IWICBitmapFrameDecode> decoder,
        WICFrame& frame) {

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

    WICBitmaps WICManager::ProcessDecoder(
        ComPtr<IWICBitmapDecoder> decoder) {

        UINT frame_count = 0;
        HRESULT hr = decoder->GetFrameCount(&frame_count);
        if (FAILED(hr) || frame_count < 1) {
            DCHECK(false);
            return {};
        }

        WICBitmaps bmps;
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
            hr = wic_factory_->CreateFormatConverter(&converter);
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

            auto wic_bitmap = converter.cast<IWICBitmapSource>();
            if (wic_bitmap == nullptr) {
                DCHECK(false);
                continue;
            }

            WICFrame wic_frame;
            GetFrameMetadata(frame_decoder, wic_frame);
            wic_frame.bitmap = wic_bitmap;

            bmps.frames.push_back(wic_frame);
        }

        if (bmps.width == 0 || bmps.height == 0) {
            if (!bmps.frames.empty()) {
                UINT width = 0;
                UINT height = 0;
                hr = bmps.frames.front().bitmap->GetSize(&width, &height);
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