#ifndef UKIVE_GRAPHICS_BITMAP_FACTORY_H_
#define UKIVE_GRAPHICS_BITMAP_FACTORY_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class WICManager;

    class BitmapFactory {
    public:
        BitmapFactory(ComPtr<ID2D1DeviceContext> d2d_dc);
        ~BitmapFactory();

        ComPtr<ID2D1Bitmap> decodeFile(const string16 &file_name);

    private:
        WICManager *wic_manager_;
        ComPtr<ID2D1DeviceContext> d2d_dc_;
    };

}

#endif // !UKIVE_GRAPHICS_BITMAP_FACTORY_H_