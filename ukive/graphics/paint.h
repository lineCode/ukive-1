#ifndef UKIVE_GRAPHICS_PAINT_H_
#define UKIVE_GRAPHICS_PAINT_H_

#include "ukive/graphics/color.h"


namespace ukive {

    class Bitmap;

    class Paint {
    public:
        enum class Style {
            STROKE,
            FILL,
            GRADIENT,
            BITMAP
        };

        enum class CapStyle {
            FLAT,
            SQUARE,
            ROUND,
            TRIANGLE,
        };

        enum class LineJoinStyle {
            MITER,
            BEVEL,
            ROUND,
            MITER_OR_BEVEL
        };

        enum DashStyle {
            SOLID,
            DASH,
            DOT,
            DASH_DOT,
            DASH_DOT_DOT,
            CUSTOM
        };

        struct StrokeStyle {
            CapStyle start_cap;
            CapStyle end_cap;
            CapStyle dash_cap;
            LineJoinStyle line_join;
            float miter_limit;
            DashStyle dash_style;
            float dash_offset;

            float* dashes;
            int dashes_count;

            StrokeStyle():
                start_cap(CapStyle::FLAT),
                end_cap(CapStyle::FLAT),
                dash_cap(CapStyle::FLAT),
                line_join(LineJoinStyle::MITER),
                miter_limit(0),
                dash_style(DashStyle::SOLID),
                dash_offset(0),
                dashes(nullptr),
                dashes_count(0) {}
        };

        Paint();
        ~Paint() = default;

        void setStyle(Style s);
        void setStrokeStyle(const StrokeStyle& s);
        void setBitmap(Bitmap* b);
        void setColor(const Color& color);
        void setOpacity(float opacity);
        void setAntialias(bool enabled);
        void setTextAntialias(bool enabled);
        void setStrokeWidth(float width);

        Style getStyle();
        StrokeStyle getStrokeStyle();
        Bitmap* getBitmap();
        Color getColor();
        float getOpacity();
        bool hasStrokeStyle();
        bool isAntialiased();
        bool isTextAntialiased();
        float getStrokeWidth();

    private:
        Color color_;
        Style style_;
        StrokeStyle stroke_style_;
        Bitmap* bitmap_;
        float opacity_;
        float stroke_width_;
        bool has_stroke_style_;
        bool is_antialiased_;
        bool is_text_antialiased_;
    };

}

#endif  // UKIVE_GRAPHICS_PAINT_H_