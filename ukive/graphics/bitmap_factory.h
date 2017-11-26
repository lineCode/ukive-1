#ifndef UKIVE_GRAPHICS_BITMAP_FACTORY_H_
#define UKIVE_GRAPHICS_BITMAP_FACTORY_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Window;

    class BitmapFactory {
    public:
        static ComPtr<ID2D1Bitmap> create(Window *win, unsigned int width, unsigned int height);
        static ComPtr<ID2D1Bitmap> decodeFile(Window *win, const string16 &file_name);
    };

}

#endif // !UKIVE_GRAPHICS_BITMAP_FACTORY_H_