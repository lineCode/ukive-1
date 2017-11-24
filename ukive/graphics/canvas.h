#ifndef UKIVE_GRAPHICS_CANVAS_H_
#define UKIVE_GRAPHICS_CANVAS_H_

#include <stack>

#include "ukive/utils/com_ptr.h"
#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class View;
    class TextRenderer;

    class Canvas
    {
    public:
        Canvas(ComPtr<ID2D1RenderTarget> renderTarget);
        ~Canvas();

        void setOpacity(float opacity);
        float getOpacity();

        void beginDraw();
        void endDraw();

        void popClip();
        void pushClip(D2D1_RECT_F &rect);

        void pushLayer(ID2D1Geometry *clipGeometry);
        void pushLayer(D2D1_RECT_F &contentBound, ID2D1Geometry *clipGeometry);
        void popLayer();

        void save();
        void restore();

        //临时方法。
        ID2D1RenderTarget *getRT();

        void scale(float sx, float sy);
        void scale(float sx, float sy, float cx, float cy);
        void rotate(float angle);
        void rotate(float angle, float cx, float cy);
        void translate(int dx, int dy);
        void translate(float dx, float dy);

        void setMatrix(D2D1::Matrix3x2F matrix);
        D2D1::Matrix3x2F getMatrix();

        void fillOpacityMask(
            float width, float height,
            ID2D1Bitmap *mask, ID2D1Bitmap *content);

        void drawRect(D2D1_RECT_F &rect, D2D1_COLOR_F &color);
        void drawRect(D2D1_RECT_F &rect, float strokeWidth, D2D1_COLOR_F &color);

        void fillRect(D2D1_RECT_F &rect, D2D1_COLOR_F &color);

        void drawRoundRect(
            D2D1_RECT_F &rect, float radius, D2D1_COLOR_F &color);
        void drawRoundRect(
            D2D1_RECT_F &rect, float strokeWidth,
            float radius, D2D1_COLOR_F &color);

        void fillRoundRect(
            D2D1_RECT_F &rect, float radius, D2D1_COLOR_F &color);

        void drawCircle(float cx, float cy, float radius, D2D1_COLOR_F &color);
        void drawCircle(float cx, float cy, float radius, float strokeWidth, D2D1_COLOR_F &color);
        void fillCircle(float cx, float cy, float radius, D2D1_COLOR_F &color);

        void drawOval(float cx, float cy, float radiusX, float radiusY, D2D1_COLOR_F &color);
        void drawOval(float cx, float cy, float radiusX, float radiusY, float strokeWidth, D2D1_COLOR_F &color);
        void fillOval(float cx, float cy, float radiusX, float radiusY, D2D1_COLOR_F &color);

        void drawBitmap(ID2D1Bitmap *bitmap);
        void drawBitmap(float x, float y, ID2D1Bitmap *bitmap);
        void drawBitmap(D2D1_RECT_F &dst, float opacity, ID2D1Bitmap *bitmap);
        void drawBitmap(D2D1_RECT_F &src, D2D1_RECT_F &dst, float opacity, ID2D1Bitmap *bitmap);

        void drawText(
            std::wstring text,
            IDWriteTextFormat *textFormat,
            D2D1_RECT_F &layoutRect, D2D1_COLOR_F &color);
        void drawTextLayout(
            float x, float y,
            IDWriteTextLayout *textLayout, D2D1_COLOR_F &color);
        void drawTextLayoutWithEffect(
            View *widget,
            float x, float y,
            IDWriteTextLayout *textLayout, D2D1_COLOR_F &color);

    private:
        int layer_counter_;
        float opacity_;
        D2D1::Matrix3x2F matrix_;

        TextRenderer *mTextRenderer;

        ComPtr<ID2D1Layer> layer_;
        ComPtr<ID2D1RenderTarget> render_target_;
        ComPtr<ID2D1SolidColorBrush> solid_brush_;
        ComPtr<ID2D1BitmapBrush> bitmap_brush_;

        std::stack<float> opacity_stack_;
        std::stack<ComPtr<ID2D1DrawingStateBlock>> drawing_state_stack_;
    };

}

#endif  // UKIVE_GRAPHICS_CANVAS_H_