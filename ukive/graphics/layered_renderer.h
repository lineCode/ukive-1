#ifndef UKIVE_GRAPHICS_LAYERED_RENDERER_H_
#define UKIVE_GRAPHICS_LAYERED_RENDERER_H_

#include <functional>

#include "ukive/graphics/drawing_object_manager.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class Canvas;
    class Window;

    class LayeredRenderer {
    public:
        bool init(Window* window);
        bool resize();
        bool render(
            Color bkColor,
            std::function<void()> renderCallback);
        void close();

    private:
        bool createRenderResource();
        void releaseRenderResource();

        Window* owner_window_;

        ComPtr<ID2D1Bitmap1> bitmap_render_target_;
        ComPtr<ID2D1DeviceContext> d2d_dc_;

        ComPtr<ID2D1Effect> shadow_effect_;
        ComPtr<ID2D1Effect> affinetrans_effect_;
    };

}

#endif  // UKIVE_GRAPHICS_LAYERED_RENDERER_H_