#ifndef UKIVE_GRAPHICS_BITMAP_FACTORY_H_
#define UKIVE_GRAPHICS_BITMAP_FACTORY_H_

#include <memory>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class Bitmap;
    class Window;

    class BitmapFactory {
    public:
        static std::shared_ptr<Bitmap> create(Window *win, unsigned int width, unsigned int height);
        static std::shared_ptr<Bitmap> decodeFile(Window *win, const string16 &file_name);
    };

}

#endif // !UKIVE_GRAPHICS_BITMAP_FACTORY_H_