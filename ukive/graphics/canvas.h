#ifndef UKIVE_GRAPHICS_CANVAS_H_
#define UKIVE_GRAPHICS_CANVAS_H_

#include <stack>
#include <memory>

#include "ukive/system/com_ptr.hpp"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/matrix.h"


namespace ukive {

    class View;
    class Rect;
    class RectF;
    class PointF;
    class Bitmap;
    class Window;
    class TextRenderer;

    class Canvas {
    public:
        Canvas(int width, int height);
        Canvas(Window* w, bool hw_acc);
        ~Canvas();

        void setOpacity(float opacity);
        float getOpacity();

        bool resize();
        bool resize(int width, int height);

        void clear();
        void clear(const Color& color);

        void beginDraw();
        void endDraw();

        void pushClip(const Rect& rect);
        void popClip();

        void pushLayer(ID2D1Geometry* clipGeometry);
        void pushLayer(const RectF& content_bound, ID2D1Geometry* clipGeometry);
        void popLayer();

        void save();
        void restore();

        //临时方法。
        ID2D1RenderTarget* getRT();

        int getWidth() const;
        int getHeight() const;
        ComPtr<ID3D11Texture2D> getTexture();
        std::shared_ptr<Bitmap> extractBitmap();

        void scale(float sx, float sy);
        void scale(float sx, float sy, float cx, float cy);
        void rotate(float angle);
        void rotate(float angle, float cx, float cy);
        void translate(float dx, float dy);

        void setMatrix(const Matrix& matrix);
        Matrix getMatrix();

        void fillOpacityMask(
            float width, float height,
            Bitmap* mask, Bitmap* content);
        void fillBitmapRepeat(const RectF& rect, Bitmap* content);

        void drawLine(
            const PointF& start, const PointF& end, const Color& color);
        void drawLine(
            const PointF& start, const PointF& end, float stroke_width, const Color& color);

        void drawRect(const RectF& rect, const Color& color);
        void drawRect(const RectF& rect, float stroke_width, const Color& color);
        void fillRect(const RectF& rect, const Color& color);

        void drawRoundRect(
            const RectF& rect, float radius, const Color& color);
        void drawRoundRect(
            const RectF& rect, float stroke_width,
            float radius, const Color& color);
        void fillRoundRect(
            const RectF& rect, float radius, const Color& color);

        void drawCircle(float cx, float cy, float radius, const Color& color);
        void drawCircle(float cx, float cy, float radius, float stroke_width, const Color& color);
        void fillCircle(float cx, float cy, float radius, const Color& color);
        void fillCircle(float cx, float cy, float radius, Bitmap* bmp);

        void drawOval(float cx, float cy, float rx, float ry, const Color& color);
        void drawOval(float cx, float cy, float rx, float ry, float stroke_width, const Color& color);
        void fillOval(float cx, float cy, float rx, float ry, const Color& color);

        void fillGeometry(ID2D1Geometry* geo, const Color& color);
        void fillGeometry(ID2D1Geometry* geo, Bitmap* bmp);

        void drawBitmap(Bitmap* bitmap);
        void drawBitmap(float x, float y, Bitmap* bitmap);
        void drawBitmap(float opacity, Bitmap* bitmap);
        void drawBitmap(const RectF& dst, float opacity, Bitmap* bitmap);
        void drawBitmap(const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap);

        void drawText(
            const string16& text,
            IDWriteTextFormat* textFormat,
            const RectF& layoutRect, const Color& color);
        void drawTextLayout(
            float x, float y,
            IDWriteTextLayout* textLayout, const Color& color);
        void drawTextLayoutWithEffect(
            View* v,
            float x, float y,
            IDWriteTextLayout* textLayout, const Color& color);

        static ComPtr<ID2D1RenderTarget> createWICRenderTarget(IWICBitmap* wic_bitmap);
        static ComPtr<ID2D1DCRenderTarget> createDCRenderTarget();

        static ComPtr<ID3D11Texture2D> createTexture2D(int width, int height, bool gdi_compat);
        static ComPtr<ID2D1RenderTarget> createDXGIRenderTarget(IDXGISurface* surface, bool gdi_compat);

        static ComPtr<IDWriteTextFormat> createTextFormat(
            const string16& font_family_name,
            float font_size, const string16& locale_name);
        static ComPtr<IDWriteTextLayout> createTextLayout(
            const string16& text, IDWriteTextFormat* format,
            float max_width, float max_height);

    private:
        void initCanvas();

        bool createOffScreenBRT(int width, int height);
        ComPtr<ID2D1RenderTarget> createHardwareBRT();
        ComPtr<ID2D1RenderTarget> createSoftwareBRT();
        ComPtr<ID2D1RenderTarget> createSwapchainBRT();

        bool resizeHardwareBRT();
        bool resizeSoftwareBRT();
        bool resizeSwapchainBRT();

        bool drawLayered();
        bool drawSwapchain();

        void releaseResources();

        int layer_counter_;
        bool is_texture_target_;
        float opacity_;
        Matrix matrix_;

        ComPtr<TextRenderer> text_renderer_;

        ComPtr<ID2D1Layer> layer_;
        ComPtr<ID2D1RenderTarget> rt_;
        ComPtr<ID2D1SolidColorBrush> solid_brush_;
        ComPtr<ID2D1BitmapBrush> bitmap_brush_;

        ComPtr<ID3D11Texture2D> d3d_tex2d_;

        std::stack<float> opacity_stack_;
        std::stack<ComPtr<ID2D1DrawingStateBlock>> drawing_state_stack_;

        bool is_layered_ = false;
        bool is_hardware_acc_ = false;
        Window* owner_window_ = nullptr;
        ComPtr<IDXGISwapChain> swapchain_;
    };

}

#endif  // UKIVE_GRAPHICS_CANVAS_H_