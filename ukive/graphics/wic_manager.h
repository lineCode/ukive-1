#ifndef UKIVE_GRAPHICS_WIC_MANAGER_H_
#define UKIVE_GRAPHICS_WIC_MANAGER_H_

#include <vector>

#include <wincodec.h>

#include "ukive/graphics/color.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    struct WICFrame {
        int left;
        int top;
        int width;
        int height;
        int disposal;
        bool interlace;
        bool sort;
        int frame_interval;
        ComPtr<IWICBitmapSource> bitmap;

        enum DisposalMethods {
            UNSPECIFIED = 0,
            NONE,
            BACKGROUND,
            PREVIOUS
        };

        WICFrame()
            :left(0), top(0),
            width(0), height(0), disposal(0),
            interlace(false), sort(false),
            frame_interval(0) {}
    };

    struct WICBitmaps {
        int width;
        int height;
        int loop_count;
        Color bg_color;
        std::vector<WICFrame> frames;

        WICBitmaps()
            :width(0), height(0), loop_count(0) {}

        bool empty() const {
            return frames.empty();
        }

        ComPtr<IWICBitmapSource> front() {
            return frames.front().bitmap;
        }
    };

    class WICManager {
    public:
        WICManager();
        ~WICManager();

        ComPtr<IWICBitmap> createBitmap(IWICBitmapSource* src);
        ComPtr<IWICBitmap> createBitmap(UINT width, UINT height);
        ComPtr<IWICBitmap> createBitmap(UINT width, UINT height, BYTE* bgra_buf);

        WICBitmaps decodeFile(const string16& file_name);
        WICBitmaps decodeMemory(const BYTE* buffer, size_t size);

        bool saveToPngFile(UINT width, UINT height, BYTE* bgra_buf, const string16& name);

    private:
        void GetGlobalMetadata(
            ComPtr<IWICBitmapDecoder> decoder,
            WICBitmaps& bmps);

        void GetFrameMetadata(
            ComPtr<IWICBitmapFrameDecode> decoder,
            WICFrame& frame);

        WICBitmaps ProcessDecoder(
            ComPtr<IWICBitmapDecoder> decoder);

        ComPtr<IWICImagingFactory> wic_factory_;
    };

}

#endif  // UKIVE_GRAPHICS_WIC_MANAGER_H_