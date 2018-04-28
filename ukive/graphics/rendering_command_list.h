#ifndef UKIVE_GRAPHICS_RENDERING_COMMAND_LIST_H_
#define UKIVE_GRAPHICS_RENDERING_COMMAND_LIST_H_

#include <vector>

#include "ukive/graphics/matrix.h"
#include "ukive/graphics/paint.h"
#include "ukive/graphics/rect.h"


namespace ukive {

    class PointF;
    class Bitmap;
    class Geometry;

    class RenderingCommandList {
    public:
        RenderingCommandList() = default;
        ~RenderingCommandList() = default;

        void save();
        void restore();

        void scale(float sx, float sy);
        void scale(float sx, float sy, float cx, float cy);
        void rotate(float angle);
        void rotate(float angle, float cx, float cy);
        void translate(float dx, float dy);
        void setMatrix(const Matrix& matrix);

        void pushClip(const RectF& rect);
        void popClip();

        void clear();
        void clear(const Color& c);

        void drawLine(const PointF& start, const PointF& end, const Paint& p);
        void drawRect(const RectF& rect, const Paint& p);
        void drawRoundRect(const RectF& rect, float radius, const Paint& p);
        void drawCircle(float cx, float cy, float radius, const Paint& p);
        void drawOval(float cx, float cy, float rx, float ry, const Paint& p);
        void drawGeometry(Geometry* geo, const Paint& p);

        void drawBitmap(Bitmap* bitmap);
        void drawBitmap(float x, float y, Bitmap* bitmap);
        void drawBitmap(float opacity, Bitmap* bitmap);
        void drawBitmap(const RectF& dst, float opacity, Bitmap* bitmap);
        void drawBitmap(const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap);

        void drawText(const string16& text, const RectF& rect, const Paint& p);

    private:
        enum class Operation {
            SAVE,
            RESTORE,
            MATRIX,
            PUSH_CLIP,
            POP_CLIP,
            CLEAR,
            DRAW_GEOMETRY,
            DRAW_BITMAP,
            DRAW_TEXT,
        };

        struct Command {
            Operation op;
            Paint paint;
            Bitmap* bitmap;
            string16 text;
            Matrix matrix;
            Geometry* geo;
        };

        std::vector<Command> cmds_;
    };

}

#endif  // UKIVE_GRAPHICS_RENDERING_COMMAND_LIST_H_