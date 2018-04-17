﻿#ifndef UKIVE_GRAPHICS_CANVAS_H_
#define UKIVE_GRAPHICS_CANVAS_H_

#include <stack>
#include <memory>

#include "ukive/utils/com_ptr.h"
#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/color.h"
#include "ukive/graphics/matrix.h"


namespace ukive {

    class View;
    class RectF;
    class Bitmap;
    class Window;
    class Renderer;
    class TextRenderer;

    class Canvas {
    public:
        Canvas(int width, int height);
        Canvas(ComPtr<ID2D1RenderTarget> renderTarget);
        ~Canvas();

        void setOpacity(float opacity);
        float getOpacity();

        void clear();
        void clear(const Color& color);

        void beginDraw();
        void endDraw();

        void popClip();
        void pushClip(const RectF& rect);

        void pushLayer(ID2D1Geometry* clipGeometry);
        void pushLayer(const RectF& content_bound, ID2D1Geometry* clipGeometry);
        void popLayer();

        void save();
        void restore();

        //临时方法。
        ID2D1RenderTarget* getRT();
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

        void drawRect(const RectF& rect, const Color& color);
        void drawRect(const RectF& rect, float strokeWidth, const Color& color);
        void fillRect(const RectF& rect, const Color& color);

        void drawRoundRect(
            const RectF& rect, float radius, const Color& color);
        void drawRoundRect(
            const RectF& rect, float strokeWidth,
            float radius, const Color& color);
        void fillRoundRect(
            const RectF& rect, float radius, const Color& color);

        void drawCircle(float cx, float cy, float radius, const Color& color);
        void drawCircle(float cx, float cy, float radius, float strokeWidth, const Color& color);
        void fillCircle(float cx, float cy, float radius, const Color& color);

        void drawCircle(const RectF& rect, const Color& color);
        void drawCircle(const RectF& rect, float strokeWidth, const Color& color);
        void fillCircle(const RectF& rect, const Color& color);

        void drawOval(float cx, float cy, float radiusX, float radiusY, const Color& color);
        void drawOval(float cx, float cy, float radiusX, float radiusY, float strokeWidth, const Color& color);
        void fillOval(float cx, float cy, float radiusX, float radiusY, const Color& color);

        void fillGeometry(ID2D1Geometry* geo, ID2D1Brush* brush);

        void drawBitmap(Bitmap* bitmap);
        void drawBitmap(float x, float y, Bitmap* bitmap);
        void drawBitmap(float opacity, Bitmap* bitmap);
        void drawBitmap(const RectF& dst, float opacity, Bitmap* bitmap);
        void drawBitmap(const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap);

        void drawText(
            std::wstring text,
            IDWriteTextFormat* textFormat,
            const RectF& layoutRect, const Color& color);
        void drawTextLayout(
            float x, float y,
            IDWriteTextLayout* textLayout, const Color& color);
        void drawTextLayoutWithEffect(
            View* v,
            float x, float y,
            IDWriteTextLayout* textLayout, const Color& color);

    private:
        void initCanvas(ComPtr<ID2D1RenderTarget> renderTarget);

        int layer_counter_;
        bool is_texture_target_;
        float opacity_;
        Matrix matrix_;

        ComPtr<TextRenderer> text_renderer_;

        ComPtr<ID2D1Layer> layer_;
        ComPtr<ID2D1RenderTarget> render_target_;
        ComPtr<ID2D1SolidColorBrush> solid_brush_;
        ComPtr<ID2D1BitmapBrush> bitmap_brush_;

        ComPtr<ID3D11Texture2D> d3d_tex2d_;

        std::stack<float> opacity_stack_;
        std::stack<ComPtr<ID2D1DrawingStateBlock>> drawing_state_stack_;
    };

}

#endif  // UKIVE_GRAPHICS_CANVAS_H_