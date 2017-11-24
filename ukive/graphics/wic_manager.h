#ifndef UKIVE_GRAPHICS_WIC_MANAGER_H_
#define UKIVE_GRAPHICS_WIC_MANAGER_H_

#include <wincodec.h>

#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class WICManager {
    public:
        WICManager();
        ~WICManager();

        HRESULT init();
        void close();

        HRESULT convertForD2D(IWICBitmapSource *src, IWICBitmapSource **dst);
        HRESULT decodeFile(const string16 &file_name, IWICBitmapSource **out_src);
        HRESULT getBitmapFromSource(IWICBitmapSource *src, IWICBitmap **out_bitmap);

    private:
        ComPtr<IWICImagingFactory> wic_factory_;
    };

}

#endif  // UKIVE_GRAPHICS_WIC_MANAGER_H_