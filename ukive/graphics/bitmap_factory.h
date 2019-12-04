#ifndef UKIVE_GRAPHICS_BITMAP_FACTORY_H_
#define UKIVE_GRAPHICS_BITMAP_FACTORY_H_

#include <memory>

#include "utils/string_utils.h"
#include "ukive/graphics/bitmap.h"
#include "ukive/graphics/color.h"


namespace ukive {

    class Bitmap;
    class Window;

    struct GifFrame {
        int left = 0;
        int top = 0;
        int width = 0;
        int height = 0;
        int disposal = 0;
        bool interlace = false;
        bool sort = false;
        int frame_interval = 0;
        SWBitmap bitmap;

        enum DisposalMethods {
            UNSPECIFIED = 0,
            NONE,
            BACKGROUND,
            PREVIOUS
        };

        GifFrame()
            : bitmap({}) {}
    };

    struct SWBitmaps {
        int width = 0;
        int height = 0;
        int loop_count = 0;
        Color bg_color;
        std::vector<GifFrame> frames;

        bool empty() const {
            return frames.empty();
        }

        SWBitmap front() {
            return frames.front().bitmap;
        }
    };

    class BitmapFactory {
    public:
        static SWBitmap create(unsigned int width, unsigned int height);
        static std::shared_ptr<Bitmap> create(
            Window *win, unsigned int width, unsigned int height);
        static std::shared_ptr<Bitmap> create(
            Window *win, unsigned int width, unsigned int height, const void* data);

        static std::shared_ptr<Bitmap> decodeFile(Window *win, const string16 &file_name);

        static SWBitmaps decodeFile(const string16& file_name);
        static SWBitmaps decodeMemory(const BYTE* buffer, DWORD size);

        static bool saveToPngFile(UINT width, UINT height, BYTE* bgra_buf, const string16& name);

    private:
        static void GetGlobalMetadata(
            ComPtr<IWICBitmapDecoder> decoder,
            SWBitmaps& bmps);

        static void GetFrameMetadata(
            ComPtr<IWICBitmapFrameDecode> decoder,
            GifFrame& frame);

        static SWBitmaps ProcessDecoder(
            ComPtr<IWICBitmapDecoder> decoder);
    };

}

#endif // !UKIVE_GRAPHICS_BITMAP_FACTORY_H_