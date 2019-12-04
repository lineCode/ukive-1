#ifndef UKIVE_GRAPHICS_CYRO_RENDERER_H_
#define UKIVE_GRAPHICS_CYRO_RENDERER_H_

#include "utils/string_utils.h"

#include "ukive/graphics/graphic_device_manager.h"


namespace ukive {

    class View;
    class Color;
    class Paint;
    class RectF;
    class PointF;
    class Bitmap;

    class CyroRenderer {
    public:
        virtual ~CyroRenderer() = default;

        virtual void clear() = 0;
        virtual void clear(const Color& c) = 0;

        virtual void pushClip(const RectF& rect) = 0;
        virtual void popClip() = 0;

        virtual void pushLayer(ID2D1Geometry* g) = 0;
        virtual void popLayer() = 0;

        virtual void save() = 0;
        virtual void restore() = 0;

        virtual void getSize(int* width, int* height) const = 0;

        virtual void scale(float sx, float sy, const PointF& c) = 0;
        virtual void rotate(float angle, const PointF& c) = 0;
        virtual void translate(float dx, float dy) = 0;

        virtual void drawPoint(const PointF& p, const Paint& paint) = 0;
        virtual void drawLine(
            const PointF& start, const PointF& end, const Paint& paint) = 0;
        virtual void drawRect(const RectF& rect, const Paint& paint) = 0;
        virtual void drawRoundRect(
            const RectF& rect, float radius, const Paint& paint) = 0;
        virtual void drawCircle(const PointF& c, float radius, const Paint& paint) = 0;
        virtual void drawEllipse(
            const PointF& c, float rx, float ry, const Paint& paint) = 0;
        virtual void drawGeometry(ID2D1Geometry* geo, const Paint& paint) = 0;
        virtual void drawBitmap(
            const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap) = 0;

        virtual void drawText(
            const string16& text, IDWriteTextFormat* format,
            const RectF& rect, const Paint& paint) = 0;
        virtual void drawTextLayout(
            float x, float y,
            IDWriteTextLayout* textLayout, const Color& color) = 0;
        virtual void drawTextLayoutWithEffect(
            View* v, float x, float y,
            IDWriteTextLayout* textLayout, const Color& color) = 0;
    };

}

#endif  // UKIVE_GRAPHICS_CYRO_RENDERER_H_